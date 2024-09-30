/*
 * Watchdog.v
 *
 * " Memory mapped watchdog circuit providing basic SW/HW reset options"
 */
module Watchdog (
	// Memory Map connections.
	input  [1:0]  i_memAddr,
	input  [15:0] i_memDataIn,
	input         i_memWrEn,
	output [15:0] o_memDataOut,
	
	// State input connections.
	input         i_smIsBooted,
	input         i_smStartPause,
	
	// Triggered reset connection.
	output        o_doReset,
	
	// Triggered interrupt connection.
	output        o_intWDT,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * Memory mapped "counter" used to reset device during SW errors.
 *
 * Implements a 2^20 cycle "fuse" before pulsing to hardware reset or interrupt
 * Only runs when NOT booting or paused. Control bits allow for stopping
 * countdown and forcing a HW reset. Writing to controls auto resets countdown.
 */
 
////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Compute control wires (based on mem address).
wire        isCtrlAddr;

// Control setting wires.
wire        cancelD, cancelQ, cancelEn;
wire        resetD, resetQ, resetEn;
wire        selectD, selectQ, selectEn;

// Compute control wires (based on counter state).
wire        isCtrlWr;
wire        doPause, doPrescaleRst, doCountInc;

// Counting registers wires.
wire [15:0] addA, addB, addS;
wire        addI, addO;
wire [15:0] cntD, cntQ;
wire        cntEn,cntRstn;
wire [3:0]  preQ;
wire        preRstn;

// Interrupt wires.
wire        intD, intQ;

// Nets for driving output data.
wire [15:0] readCtrl;
wire [15:0] readAddr0X;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Control Wires- saved controls for disabling watchdog and resetting uP.
DffSynchEn CANCEL_REG (
	.D(cancelD),
	.Q(cancelQ),
	.S(cancelEn),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn RESET_REG (
	.D(resetD),
	.Q(resetQ),
	.S(resetEn),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn SELECT_REG (
	.D(selectD),
	.Q(selectQ),
	.S(selectEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Counting registers- register + logic used to track passed time.
AddCout16 INC (
	.A(addA),
	.B(addB),
	.I(addI),
	.O(addO),
	.S(addS)
);
DffSynchEn CNT[15:0] (
	.D(cntD),
	.Q(cntQ),
	.S(cntEn),
	.clk(i_clk),
	.rstn(cntRstn)
);
RegInc4 PRESCALE (
	.Q(preQ),
	.clk(i_clk),
	.rstn(preRstn)
);

//------------------------------------------------------------------------------
// Interupt- asserts interrupt on bark (if configured to do so).
DffSynch INT (
	.D(intD),
	.Q(intQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute address based controls.
assign isCtrlAddr = ~i_memAddr[1] & ~i_memAddr[0]; // ...00

//------------------------------------------------------------------------------
// Handle Control inputs.
assign cancelD  = i_memDataIn[0];                          // bit 0
assign cancelEn = isCtrlAddr & i_memWrEn;
assign resetD   = i_memDataIn[1];                          // bit 1
assign resetEn  = isCtrlAddr & i_memWrEn;
assign selectD  = i_memDataIn[3];                          // bit 3
assign selectEn = isCtrlAddr & i_memWrEn;

//------------------------------------------------------------------------------
// Compute counter state based controls.
assign isCtrlWr      = isCtrlAddr & i_memWrEn;
assign doPause       = ~i_smIsBooted | i_smStartPause;
assign doPrescaleRst = i_rstn & ~doPause & ~isCtrlWr;
assign doCountInc    = (&preQ) & ~cancelQ & ~doPause;

//------------------------------------------------------------------------------
// Handle counting register/logic inputs.
assign addA    = cntQ;
assign addB    = 16'b0000000000000000;   // using as inc
assign addI    = 1'b1;                   // using as inc
assign cntD    = addS;
assign cntEn   = doCountInc;
assign cntRstn = i_rstn & ~isCtrlWr;
assign preRstn = doPrescaleRst;

//------------------------------------------------------------------------------
// Handle interrupt wires.
assign intD = &cntQ & doCountInc & selectQ; // sel = interrupt

//------------------------------------------------------------------------------
// Drive data output based on given address.
assign readCtrl = {doPause | cancelQ,
                   11'b00000000000,
						 selectQ,
						 1'b0,
						 resetQ,
						 cancelQ};
Mux2 M0[15:0] (
	.A(cntQ),                                               // Addr 01? CNT Reg
	.B(readCtrl),                                           // Addr 00? CTRL Reg
	.S(i_memAddr[0]),
	.Y(readAddr0X)
);
assign o_memDataOut = readAddr0X & {16{~i_memAddr[1]}};    // Addr 1X? Zeroes

//------------------------------------------------------------------------------
// Drive reset output.
assign o_doReset = resetQ | (&cntQ & doCountInc & ~selectQ); // ~sel = reset

//------------------------------------------------------------------------------
// Drive interrupt output.
assign o_intWDT = intQ;

endmodule
