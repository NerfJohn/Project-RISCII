/*
 * Alu.v
 *
 * "Heart of the processor's computational abilities"
 */
module Alu (
	// Operand connections.
	input  [15:0] i_srcA,
	input  [15:0] i_srcB,
	
	// Opcode connections.
	//input  [2:0]  i_opCode,
	input         i_opSel,
	
	// Results connections.
	output [15:0] o_result,
	output [3:0]  o_ccodes
);

/*
 * TODO- desc
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// LBI/"Set" computation wires.
wire [15:0] setResult;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
	
////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute "set" result (ie LBI).
Mux2 M0[15:0] (
	.A({i_srcA[7:0], i_srcB[7:0]}), // LBI w/ flag? "Shift" immediate in
	.B(i_srcB),                     // No?          Just use full immediate
	.S(i_opSel),
	.Y(setResult)
);

//------------------------------------------------------------------------------
// TODO- implement.
assign o_result = setResult;
assign o_ccodes = {setResult[15], ~(|setResult), ~setResult[15], 1'b0};

endmodule
