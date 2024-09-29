/*
 * I2C.v
 *
 * "Mapped I2C controller capable of sending bytes with configurable overhead"
 */
module I2C (
	// Memory Map connections.
	input  [1:0]  i_memAddr,
	input  [15:0] i_memDataIn,
	input         i_memWrEn,
	output [15:0] o_memDataOut,
	
	// State input connections.
	input         i_smIsBooted,
	input         i_smStartPause,
	output        o_smNowPaused,
	
	// Serial pin connections.
	input         i_pinSDAIn,
	output        o_pinSCLDir,
	output        o_pinSDADir,
	
	// Interrupt connection.
	output        o_intI2C,

	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * TODO- desc
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Compute control wires (based on mem access/uP state).
wire        isCtrlWr, isBaudWr, isDataWr;
wire        doPause;

// CTRL register wires.
wire [3:0]  ctrlD, ctrlQ;
wire        ctrlS;
wire        cfgStart, cfgWr, cfgStop, cfgEnable;

// BAUD register wires.
wire [14:0] baudD, baudQ;
wire        baudS;

// DATA register wires.
wire [7:0]  dataD, dataQ;
wire        dataS;

// Compute control wires (based on core process).
wire        isIdle, isHead, isAck, isTail;
wire        atCntHalf, atCntMax;
wire        doStart, doShift;

// Core process wires.
wire [15:0] cntQ;
wire        cntRstn;
wire        halfD, halfQ, halfS;
wire [3:0]  addA, addB, addS;
wire        addI;
wire [3:0]  stateD, stateQ;
wire        stateS;

// Pin logic wires.
wire        sclD, sclQ, sclS, sclRstn;
wire        sclHeadTail, sclNoHeadTail, sclDir;
wire        sdaD, sdaQ, sdaS, sdaRstn;
wire        sdaHeadTail, sdaNoHeadTail;

// ACK capture wires.
wire        ackD, ackQ, ackS;

// Interrupt wires.
wire        intD, intQ;

// Pause wires.
wire        pauseD, pauseQ;

// Register read wires.
wire [15:0] rdCtrl;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// CTRL register- writable controls for the I2C peripheral.
DffSynchEn CTRL[3:0] (
	.D(ctrlD),
	.Q(ctrlQ),
	.S(ctrlS),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// BAUD register- writable parameter defining 0.5 of I2C baud rate.
DffSynchEn BAUD[14:0] (
	.D(baudD),
	.Q(baudQ),
	.S(baudS),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// DATA register- register used to read/write data from I2C.
DffSynchEn DATA[7:0] (
	.D(dataD),
	.Q(dataQ),
	.S(dataS),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Core Process- pieces used to implement core state machine of I2C.
RegInc16 BAUD_COUNTER (
	.Q(cntQ),
	.clk(i_clk),
	.rstn(cntRstn)       // custom reset behavior
);
DffSynchEn HALF_PULSE (
	.D(halfD),
	.Q(halfQ),
	.S(halfS),
	.clk(i_clk),
	.rstn(i_rstn)
);
Add4 STATE_INC (
	.A(addA),
	.B(addB),
	.I(addI),
	.S(addS)
);
DffSynchEn STATE[3:0] (
	.D(stateD),
	.Q(stateQ),
	.S(stateS),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Pin Logic- registers used to output the correct function of each pin.
DffSynchEn SCL_DIR (
	.D(sclD),
	.Q(sclQ),
	.S(sclS),
	.clk(i_clk),
	.rstn(sclRstn)    // custom reset behavior
);
DffSynchEn SDA_DIR (
	.D(sdaD),
	.Q(sdaQ),
	.S(sdaS),
	.clk(i_clk),
	.rstn(sdaRstn)    // custom reset behavior
);

//------------------------------------------------------------------------------
// ACK register- record ACK status of last message.
DffSynchEn ACK (
	.D(ackD),
	.Q(ackQ),
	.S(ackS),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Interrupts- pulsed upon one byte of transfer.
DffSynch INT_I2C (
	.D(intD),
	.Q(intQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Pause register- UART formally/locally pauses once both channels are idle.
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
assign isCtrlWr = ~i_memAddr[1] & ~i_memAddr[0] & i_memWrEn; // ...00
assign isBaudWr = ~i_memAddr[1] &  i_memAddr[0] & i_memWrEn; // ...01
assign isDataWr =  i_memAddr[1] & ~i_memAddr[0] & i_memWrEn; // ...10
assign doPause  = ~i_smIsBooted | i_smStartPause | pauseQ;

//------------------------------------------------------------------------------
// Handle CTRL register inputs.
assign ctrlD     = i_memDataIn[3:0];
assign ctrlS     = isCtrlWr;
assign cfgStart  = ctrlQ[3];         // bit 3
assign cfgWr     = ctrlQ[2];         // bit 2
assign cfgStop   = ctrlQ[1];         // bit 1
assign cfgEnable = ctrlQ[0];         // bit 0

//------------------------------------------------------------------------------
// Handle BAUD register inputs.
assign baudD = i_memDataIn[15:1];
assign baudS = isBaudWr;

//------------------------------------------------------------------------------
// Handle DATA register inputs.
Mux2 M0[7:0] (
	.A(i_memDataIn[7:0]),           // Mem Wr? use mem data
	.B({dataQ[6:0], i_pinSDAIn}),   // No?     use shift
	.S(isDataWr),
	.Y(dataD)
);
assign dataS = isDataWr | doShift; // Mem | sample

//------------------------------------------------------------------------------
// Compute core process related control wires.
assign isIdle    = ~(|stateQ);
assign isHead    = ~stateQ[3] & ~stateQ[2] & ~stateQ[1] &  stateQ[0];
assign isAck     =  stateQ[3] & ~stateQ[2] &  stateQ[1] & ~stateQ[0];
assign isTail    =  stateQ[3] & ~stateQ[2] &  stateQ[1] &  stateQ[0];
assign atCntHalf = ~(|(cntQ[14:0] ^ baudQ));
assign atCntMax  = ~(|(cntQ[15:1] ^ baudQ));
assign doStart   = cfgEnable & isIdle & isDataWr & ~doPause;
assign doShift   = ~isIdle & atCntHalf & halfQ & ~isHead & ~isAck & ~isTail;

//------------------------------------------------------------------------------
// Handle core process inputs.
assign cntRstn = i_rstn & ~isIdle & ~atCntMax;
assign halfD   = ~halfQ;                                 // oscillate...
assign halfS   = ~isIdle & atCntMax;                     // ...during process
assign addA    = stateQ;
assign addB    = 4'b0000;                                // using as inc
assign addI    = 1'b1;                                   // using as inc
assign stateD  = addS & ~{4{isTail}};
assign stateS  = doStart | (~isIdle & halfQ & atCntMax); // 2 halves per state

//------------------------------------------------------------------------------
// Handle pin logic inputs.
assign sclD    = ~cfgStop;               // de-assert based on stop condition
assign sclS    = ~isIdle & atCntMax;	  // update on way to idle
assign sclRstn = i_rstn & cfgEnable;     // zero on HW or I2C reset
Mux2 M1 (
	.A(~(cfgStart & halfQ)),              // Head? use "start" logic
	.B(~(cfgStop & halfQ)),               // No? use "stop" logic
	.S(isHead),
	.Y(sclHeadTail)
);
Mux2 M2 (
	.A(sclQ),                             // Idle? use saved scl
	.B(~halfQ),                           // No? use "default" logic
	.S(isIdle),
	.Y(sclNoHeadTail)
);
Mux2 M3 (
	.A(sclHeadTail),                      // Head/Tail? process further
	.B(sclNoHeadTail),                    // No?        use alts
	.S(isHead | isTail),
	.Y(sclDir)
);
Mux2 M4 (
	.A(cfgStart & halfQ),                 // Head? use "start" logic
	.B(cfgStop & ~halfQ),                 // No?   use "stop" logic
	.S(isHead),
	.Y(sdaHeadTail)
);
Mux2 M5 (
	.A(~cfgWr),                           // Ack? use "ack" logic
	.B(~dataQ[7] & cfgWr),                // No?  use default logic
	.S(isAck),
	.Y(sdaNoHeadTail)
);
Mux2 M6 (
	.A(sdaHeadTail),                      // Head/Tail? process further
	.B(sdaNoHeadTail),                    // No?        use alts
	.S(isHead |isTail),
	.Y(sdaD)
);
assign sdaS    = ~isIdle & atCntHalf;
assign sdaRstn = i_rstn & cfgEnable;

//------------------------------------------------------------------------------
// Handle ACK record inputs.
assign ackD = i_pinSDAIn;
assign ackS = isAck & atCntHalf & halfQ; // after ACK posedge

//------------------------------------------------------------------------------
// Handle interrupt input.
assign intD = isTail & halfQ & atCntMax & cfgEnable & ~doPause;

//------------------------------------------------------------------------------
// Handle pause input.
assign pauseD = i_smStartPause & isIdle;

//------------------------------------------------------------------------------
// Drive pause connection.
assign o_smNowPaused = pauseQ;

//------------------------------------------------------------------------------
// Drive mem data output based on given address.
assign rdCtrl = {isIdle,     // I2C idle status
					  ackQ,       // Last byte's ACK status
					  10'b0,
					  ctrlQ};     // Controls
Mux4 M7[15:0] (
	.C(rdCtrl),               // Address 00? Read controls/statuses
	.D({baudQ, 1'b0}),        // Address 01? Read baud rate
	.E({8'b00000000, dataQ}), // Address 10? Read data byte
	.F(16'b0000000000000000), // Address 11? no register
	.S(i_memAddr),
	.Y(o_memDataOut)
);

//------------------------------------------------------------------------------
// Drive I2C pin connections.
assign o_pinSCLDir = sclDir;
assign o_pinSDADir = sdaQ;

//------------------------------------------------------------------------------
// Drive interrupt connection.
assign o_intI2C = intQ;

endmodule
