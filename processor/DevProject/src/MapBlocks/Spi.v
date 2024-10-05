/*
 * Spi.v
 *
 * "Memory mapped SPI controller dedicated to storage chip communication.
 */
module Spi (
	// Memory Map connections.
	input  [1:0]  i_memAddr,
	input  [15:0] i_memDataIn,
	input         i_memWrEn,
	output [15:0] o_memDataOut,
	
	// Serial connections.
	input         i_spiMISO,
	output        o_spiMOSI,
	output        o_spiEn,
	output        o_spiHold,
	
	// State input connections.
	input         i_smIsBooted,
	input         i_smStartPause,
	output        o_smNowPaused,

	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * SPI Controller explictly connected to uP's storage chip.
 *
 * Allows for byte length TX/RX of data. SPI uses lock/key instead of enable bit
 * to make it harder for rouge or corrupted code mess with the storage chip.
 * Unlocking  SPI activates storage chip, though clocking is only done on TX/RX.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Compute control wires (based on mem access/uP state).
wire        isLockWr, isDataWr;
wire        doPause;

// LOCK wires.
wire [15:0] lockD, lockQ;
wire        lockS;
wire        isUnlocked;

// DATA wires.
wire [7:0]  dataD, dataQ;
wire        dataS;

// Compute control wires (based on SPI state machine).
wire        isIdle, isEnd;
wire        doStart;

// State machine wires.
wire [3:0]  stateQ;
wire        stateRstn;

// Pause wires.
wire        pauseD, pauseQ;

// Mem read wires.
wire [15:0] readCtrlReg;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// LOCK register- register preventing SPI use until proper "key" is set.
DffSynchEn LOCK[15:0] (
	.D(lockD),
	.Q(lockQ),
	.S(lockS),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// DATA register- register used to send and receive byte across SPI connection.
DffSynchEn DATA[7:0] (
	.D(dataD),
	.Q(dataQ),
	.S(dataS),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// State machine- counts bits as their shifted out, idling in-between bytes.
RegInc4 STATE (
	.Q(stateQ),
	.clk(i_clk),
	.rstn(stateRstn)
);

//------------------------------------------------------------------------------
// Pause register- SPI formally/locally pauses once both channels are idle.
DffSynch PAUSE (
	.D(pauseD),
	.Q(pauseQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute mem access related control wires.
assign isLockWr = ~i_memAddr[1] &  i_memAddr[0] & i_memWrEn; // ...01
assign isDataWr =  i_memAddr[1] & ~i_memAddr[0] & i_memWrEn; // ...10
assign doPause  = ~i_smIsBooted | i_smStartPause | pauseQ;

//------------------------------------------------------------------------------
// Handle LOCK register inputs.
assign lockD      = i_memDataIn;
assign lockS      = isLockWr;
assign isUnlocked = ~(|{lockQ[15:14], // Must be 0x2024 to unlock
                        lockQ[12:6],
								lockQ[4:3],
								lockQ[1:0]})
						  & &{lockQ[13],
						      lockQ[5],
								lockQ[2]};

//------------------------------------------------------------------------------
// Handle DATA resgister inputs.
Mux2 M0[7:0] (
	.A(i_memDataIn[7:0]),
	.B({dataQ[6:0], i_spiMISO}),
	.S(isDataWr),
	.Y(dataD)
);
assign dataS = (isDataWr & isUnlocked) | ~isIdle;

//------------------------------------------------------------------------------
// Compute state related control wires.
assign isIdle  = ~(|stateQ);
assign isEnd   = stateQ[3] & ~stateQ[2] & ~stateQ[1] & ~stateQ[0];
assign doStart = isUnlocked & isIdle & isDataWr & ~doPause;

//------------------------------------------------------------------------------
// Handle SPI state inputs.
assign stateRstn = i_rstn & ~isEnd & (~isIdle | doStart);

//------------------------------------------------------------------------------
// Handle SPI pause inputs.
assign pauseD = i_smStartPause & isIdle;

//------------------------------------------------------------------------------
// Drive data output based on given address.
assign readCtrlReg = {isIdle,              // idle status of SPI
                      isUnlocked,          // lock status of SPI
							 14'b00000000000000};
Mux4 M1[15:0] (
	.C(readCtrlReg),                        // Address 00? Read statuses
	.D(lockQ),                              // Address 01? Read lock
	.E({8'b00000000, dataQ}),               // Address 10? Read data
	.F(16'b0000000000000000),               // Address 11? No register
	.S(i_memAddr),
	.Y(o_memDataOut)
);

//------------------------------------------------------------------------------
// Drive serial connection.
assign o_spiMOSI = dataQ[7];
assign o_spiEn   = isUnlocked; // set CS to "active" based on lock
assign o_spiHold = isIdle;     // prevent clock output between bytes

//------------------------------------------------------------------------------
// Drive state outputs.
assign o_smNowPaused = pauseQ;

endmodule
