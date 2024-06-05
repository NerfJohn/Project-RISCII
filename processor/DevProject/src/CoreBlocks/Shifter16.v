/*
 * Shifter16.v
 *
 * "Barrel shifter specialized for core's specific shift instructions"
 */
module Shifter16 (
	// Operand inputs.
	input  [15:0] i_lhs,
	input  [3:0]  i_rhs,
	
	// Control input.
	input  [1:0]  i_opcode,
	
	// Result of shifting.
	output [15:0] o_result
);

/*
 * TODO- desc. module (0-3 = no shift
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------

endmodule
