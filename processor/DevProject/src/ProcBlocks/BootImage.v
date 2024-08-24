/*
 * BootImage.v
 *
 * "Reads image to copy sections from storage to runtime chip"
 */
module BootImage (
	// Storage chip connections.
	input         i_spiMISO,
	output        o_spiMOSI,
	output        o_spiEn,
	
	// Runtime chip connections.
	output [15:0] o_memAddr,
	output [15:0] o_memDataOut,
	output        o_memEn,
	
	// Asserted state signal.
	output        o_smNowBooted,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * Bootloader circuit to copy text/data sections to the runtime chip.
 *
 * Reads out binary image, using its metadata to copy the text and data sections
 * into their respective runtime chip locations. Circuit is designed to always
 * terminate and emit a signal to signify the runtime memory is ready.
 *
 * Design Notes:
 * 1) Requires quite a bit of knowledge about the SPI storage chip to work.
 * 2) Source of "isBooted" signal for uP's state machine.
 * 3) Relies on binary image metadata to how size of each section to copy.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Bit counter wires.
wire [3:0]  bitCntQ;
wire        bitCntRstn;

// Computed controls (based on bit counter).
wire        isCnt7, isCnt15;

// State machine wires.
wire [2:0]  smAddA, smAddB, smAddS;
wire        smAddI;
wire [2:0]  smStateD, smStateQ;
wire        smStateEn;
wire        smState0XX, smState1XX;

// Computed controls (based on state machine).
wire        doSpiEn, doClrAddr;
wire        inStartState, inAddrState, inFinalState, inSaveState;
wire        inWriteState;

// SPI shift register wires.
wire [15:0] shiftD, shiftQ;

// Memory enable wires.
wire        memEnD, memEnQ;

// Memory address wires.
wire [14:0] addrAddA, addrAddB, addrAddS;
wire        addrAddI;
wire [14:0] addrD, addrQ;
wire        addrEn;
wire        addrRstn;

// Comparator wires.
wire        saveD, saveQ;
wire [14:0] compD, compQ;
wire        compEn;

// Compute controls (based on address and comparator).
wire        isMatch;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Bit Counter- used to counts bits shifted from storage chip.
RegInc4 BIT_COUNT (
	.Q(bitCntQ),
	.clk(i_clk),
	.rstn(bitCntRstn) // custom reset behavior
);

//------------------------------------------------------------------------------
// State Machine- incrementing state value w/ enable controlling when.
Add3 STATE_INC (
	.A(smAddA),
	.B(smAddB),
	.I(smAddI),
	.S(smAddS)
);
DffSynchEn STATE_MACHINE[2:0] (
	.D(smStateD),
	.Q(smStateQ),
	.S(smStateEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Shift Register- used to shift data in/out of the storage chip.
DffSynch SHIFT[15:0] (
	.D(shiftD),
	.Q(shiftQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Memory Enable- causes contents of shift register to be written to memory.
DffSynch MEM_ENABLE (
	.D(memEnD),
	.Q(memEnQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Memory Address- (most of) address being used and incremented per write.
Add15 ADDR_INC (
	.A(addrAddA),
	.B(addrAddB),
	.I(addrAddI),
	.S(addrAddS)
);
DffSynchEn MEM_ADDR[14:0] (
	.D(addrD),
	.Q(addrQ),
	.S(addrEn),
	.clk(i_clk),
	.rstn(addrRstn) // custom reset behavior
);

//------------------------------------------------------------------------------
// Comparator- holds value to compare against write address to advance state.
DffSynch SAVE_COMP (
	.D(saveD),
	.Q(saveQ),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn COMPARATOR[14:0] (
	.D(compD),
	.Q(compQ),
	.S(compEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Handle bit counter reset behavior.
assign bitCntRstn = i_rstn & ~smStateEn; // Renew per state/reset (or overflow)

//------------------------------------------------------------------------------
// Compute bit counter based controls.
assign isCnt7  = ~bitCntQ[3] & (&bitCntQ[2:0]);
assign isCnt15 = &bitCntQ[3:0];

//------------------------------------------------------------------------------
// Handle state machine inputs.
assign smAddA   = smStateQ;
assign smAddB   = 3'b000;   // using as incrementor
assign smAddI   = 1'b1;     // using as incrementor
assign smStateD = smAddS;
Mux4 M0 (
	.C(1'b1),                    // State START     (000)? Proceed to next state
	.D(isCnt7),                  // State READ_CMD  (001)? Send 8-bit cmd
	.E(isCnt15),                 // State READ_ADDR (010)? Send 16-bit address
	.F(isCnt15),                 // State TEXT_META (011)? Read 16-bit metadata
	.S(smStateQ[1:0]),
	.Y(smState0XX)
);
Mux4 M1 (
	.C(isCnt15 & isMatch),       // State TEXT_DATA (100)? Copy text section
	.D(isCnt15),                 // State DATA_META (101)? Read 16-bit metadata
	.E(isCnt15 & isMatch),       // State DATA_DATA (110)? Copy data section
	.F(1'b0),                    // State END       (111)? Loop forever
	.S(smStateQ[1:0]),
	.Y(smState1XX)
);
Mux2 M2 (
	.A(smState1XX),
	.B(smState0XX),
	.S(smStateQ[2]),
	.Y(smStateEn)
);

//------------------------------------------------------------------------------
// Compute state machine based controls.
assign doSpiEn      = (|smStateQ) & ~(&smStateQ);                 // 001-110
assign doClrAddr    =  smStateQ[0];                               // xx1
assign inStartState = ~smStateQ[2] & ~smStateQ[1] & ~smStateQ[0]; // 000
assign inAddrState  = ~smStateQ[2] &  smStateQ[1] & ~smStateQ[0]; // 010
assign inWriteState =  smStateQ[2] & ~smStateQ[0];                // 1x0
assign inFinalState =  smStateQ[2] &  smStateQ[1] &  smStateQ[0]; // 111
assign inSaveState  =  smStateQ[0];                               // xx1

//------------------------------------------------------------------------------
// Handle shift register inputs.
Mux2 M3[15:0] (
	.A({8'b00000011, 8'b00000000}), // Boot Start? Load SPI "Read" command
	.B({shiftQ[14:0], i_spiMISO}),  // No?         Shift in SPI storage data
	.S(inStartState),
	.Y(shiftD)
);

//------------------------------------------------------------------------------
// Handle memory enable input.
assign memEnD = isCnt15 & inWriteState;

//------------------------------------------------------------------------------
// Handle memory address inputs.
assign addrAddA = addrQ;
assign addrAddB = 15'b000000000000000; // using as incrementor
assign addrAddI = 1'b1;                // using as incrementor
assign addrD    = addrAddS;
assign addrEn   = memEnQ;              // inc address after write
assign addrRstn = i_rstn & ~doClrAddr; // reset just before write states

//------------------------------------------------------------------------------
// Handle comparator inputs.
assign saveD  = isCnt15 & inSaveState;
assign compD  = shiftQ[14:0];          // top bit is assumed by section
assign compEn = saveQ;

//------------------------------------------------------------------------------
// Compute address/comparator based controls.
assign isMatch = ~(|(addrQ ^ compQ));

//------------------------------------------------------------------------------
// Drive SPI storage outputs.
assign o_spiMOSI = shiftQ[15] & ~inAddrState;
assign o_spiEn   = doSpiEn;

//------------------------------------------------------------------------------
// Drive memory write outputs.
assign o_memAddr    = {smStateQ[1], addrQ}; // section determines addr half
assign o_memDataOut = shiftQ;
assign o_memEn      = memEnQ;

//------------------------------------------------------------------------------
// Drive state machine output.
assign o_smNowBooted = inFinalState & ~memEnQ; // finish write before done

endmodule
