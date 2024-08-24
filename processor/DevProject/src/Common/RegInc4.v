/*
 * RegInc4.v
 *
 * "Auto incrementing 4-bit register"
 */
module RegInc4 (
	// Output count connection.
	output [3:0]  Q,
	
	// Common signals.
	input         clk,
	input         rstn
);

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Count register wires.
wire [3:0]  cntD, cntQ;

// Incrementor adder wires.
wire [3:0] addA, addB, addS;
wire       addI;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Count register- holds current count of the register.
DffSynch COUNT[3:0] (
	.D(cntD),
	.Q(cntQ),
	.clk(clk),
	.rstn(rstn)
);

//------------------------------------------------------------------------------
// Incrementing adder- generic adder (w/o carry out) intended for incrementing.
Add4 ADD_INC (
	.A(addA),
	.B(addB),
	.I(addI),
	.S(addS)
);


////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Handle count register inputs.
assign cntD = addS;

//------------------------------------------------------------------------------
// Handle incrementing adder inputs.
assign addA = cntQ;
assign addB = 4'b0000; // using as incrementor
assign addI = 1'b1;    // using as incrementor

//------------------------------------------------------------------------------
// Drive output of register.
assign Q = cntQ;

endmodule
