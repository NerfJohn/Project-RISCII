/*
 * CoreCtrl.v
 *
 * "Satellite status/controls for the uP's core"
 */
module CoreCtrl (
	// Memory Map connections.
	input  [1:0]  i_memAddr,
	input  [15:0] i_memDataIn,
	input         i_memWrEn,
	output [15:0] o_memDataOut,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * TODO- desc once all funcs are made.
 *		(enable signals/stack setting does nothing at the moment)
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Compute control wires (based on mem address).
wire        isCtrlAddr, isSetpointAddr;

// Control setting wires.
wire        overflowD, overflowQ, overflowEn;
wire        pauseD, pauseQ, pauseEn;

// Overflow setpoint wires.
wire [15:0] setpointD, setpointQ;
wire        setpointEn;

// Compute data wires (based on mem address).
wire [15:0] readAddr0X;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Control Settings- various read/write settings to control core options.
DffSynchEn OVERFLOW_EN (
	.D(overflowD),
	.Q(overflowQ),
	.S(overflowEn),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn PAUSE_EN (
	.D(pauseD),
	.Q(pauseQ),
	.S(pauseEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Overflow Setpoint- value to compare with to determine if stack overflowed.
DffSynchEn SETPOINT[15:0] (
	.D(setpointD),
	.Q(setpointQ),
	.S(setpointEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute address based controls.
assign isCtrlAddr     = ~i_memAddr[1] & ~i_memAddr[0]; // ...00
assign isSetpointAddr = ~i_memAddr[1] &  i_memAddr[0]; // ...01

//------------------------------------------------------------------------------
// Handle control settings inputs.
assign overflowD  = i_memDataIn[1];         // Overflow Detect Enable == bit 1
assign overflowEn = isCtrlAddr & i_memWrEn;
assign pauseD     = i_memDataIn[0];         // Do Pause == bit 0
assign pauseEn    = isCtrlAddr & i_memWrEn;

//------------------------------------------------------------------------------
// Handle setpoint inputs.
assign setpointD  = i_memDataIn;
assign setpointEn = isSetpointAddr & i_memWrEn;

//------------------------------------------------------------------------------
// Drive data output based on given address.
Mux2 M0[15:0] (
	.A(setpointQ),                               // Address 01? Read setpoint
	.B({14'b00000000000000, overflowQ, pauseQ}), // No    (00)? Read controls
	.S(i_memAddr[0]),
	.Y(readAddr0X)
);
assign o_memDataOut = readAddr0X & {16{isCtrlAddr | isSetpointAddr}};

endmodule
