/*
 * Timer.v
 *
 * "Basic 16-bit timer/counter with prescale and max count"
 */
module Timer (
	// Memory Map connections.
	input  [1:0]  i_memAddr,
	input  [15:0] i_memDataIn,
	input         i_memWrEn,
	output [15:0] o_memDataOut,
	
	// State input connections.
	input         i_smIsBooted,
	input         i_smStartPause,
	
	// Interrupt connection.
	output        o_intTMR,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * Simple 16-bit count up timer with prescale, max count, and interrupt.
 *
 * For predictability, the prescale count is reset for every write to the
 * control register and pause. Similarly, the count is reset for every write
 * of the max count register. Interrupt pulses w/ adjusted max count.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Compute control wires (based on mem address).
wire        isCtrlAddr, isCntAddr, isMaxAddr;

// Compute control wires (based on counter state).
wire        doPause;
wire        isCtrlWr, isCntWr, isMaxWr;
wire        isPreMatch, isMaxMatch;
wire        doInc;

// Control settings wires.
wire        enableD, enableQ, enableEn;
wire [3:0]  preD, preQ;
wire        preEn;

// Prescale wires.
wire [3:0]  scaleQ;
wire        scaleRstn;

// Counter wires.
wire [15:0] addCntA, addCntB, addCntS;
wire        addCntI;
wire [15:0] cntD, cntQ;
wire        cntEn, cntRstn;

// Max count wires.
wire [15:0] maxD, maxQ;
wire        maxEn;

// Interrupt wires.
wire        intD, intQ;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Control Settings- saved settings determining the timer's operation.
DffSynchEn ENABLE (
	.D(enableD),
	.Q(enableQ),
	.S(enableEn),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn PRESCALE_SETTING[3:0] (
	.D(preD),
	.Q(preQ),
	.S(preEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Presacle/Prescalar- counter that must finish prior to count incrementing.
RegInc4 PRESCALAR (
	.Q(scaleQ),
	.clk(i_clk),
	.rstn(scaleRstn)
);

//------------------------------------------------------------------------------
// Count Register- holds current count of the timer.
Add16 INC (
	.A(addCntA),
	.B(addCntB),
	.I(addCntI),
	.S(addCntS)
);
DffSynchEn COUNT[15:0] (
	.D(cntD),
	.Q(cntQ),
	.S(cntEn),
	.clk(i_clk),
	.rstn(cntRstn)
);

//------------------------------------------------------------------------------
// Max Count Register- holds the highest allowable count value.
DffSynchEn MAX[15:0] (
	.D(maxD),
	.Q(maxQ),
	.S(maxEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Interrupt- triggered as timer overflows back to zero.
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
assign isCntAddr  = ~i_memAddr[1] &  i_memAddr[0]; // ...01
assign isMaxAddr  =  i_memAddr[1] & ~i_memAddr[0]; // ...10

//------------------------------------------------------------------------------
// Compute state based controls.
assign doPause    = ~i_smIsBooted | i_smStartPause;
assign isCtrlWr   = isCtrlAddr & i_memWrEn;
assign isCntWr    = isCntAddr  & i_memWrEn;
assign isMaxWr    = isMaxAddr  & i_memWrEn;
assign isPreMatch = ~(|(preQ ^ scaleQ));
assign isMaxMatch = ~(|(maxQ ^ cntQ));
assign doInc      = isPreMatch & enableQ & ~doPause;

//------------------------------------------------------------------------------
// Handle control settings inputs.
assign enableD  = i_memDataIn[0];
assign enableEn = isCtrlWr;
assign preD     = i_memDataIn[7:4];
assign preEn    = isCtrlWr;

//------------------------------------------------------------------------------
// Handle prescalar inputs.
assign scaleRstn = i_rstn & ~isPreMatch & ~isCtrlWr & ~doPause;

//------------------------------------------------------------------------------
// Handle count register inputs.
assign addCntA = cntQ;
assign addCntB = 16'b0000000000000000;           // using as inc
assign addCntI = 1'b1;                           // using as inc
Mux2 M0[15:0] (
	.A(i_memDataIn),                              // Data Write? Use access data
	.B(addCntS & ~{16{isMaxMatch | isMaxWr}}),    // No?         Use next inc
	.S(isCntWr),
	.Y(cntD)
);
assign cntEn   = doInc | isCntWr;
assign cntRstn = i_rstn & ~isMaxWr;

//------------------------------------------------------------------------------
// Handle max count inputs.
assign maxD  = i_memDataIn;
assign maxEn = isMaxWr;

//------------------------------------------------------------------------------
// Handle interrupt inputs.
assign intD = isMaxMatch & doInc;

//------------------------------------------------------------------------------
// Drive data output based on given address.
Mux4 M1[15:0] (
	.C({8'b00000000, preQ, 3'b000, enableQ}), // Address 00? Read controls
	.D(cntQ),                                 // Address 01? Read count
	.E(maxQ),                                 // Address 10? Read max count
	.F({16'b0000000000000000}),               // Address 11? no register
	.S(i_memAddr),
	.Y(o_memDataOut)
);

//------------------------------------------------------------------------------
// Drive interrupt signal.
assign o_intTMR = intQ;

endmodule
