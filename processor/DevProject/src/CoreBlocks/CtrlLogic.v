/*
 * CtrlLogic.v
 *
 * "Computed control signals based on a given instruction opcode"
 */
module CtrlLogic (
	// Input opcode connection.
	input  [3:0]  i_opcode,
	
	// Control outputs.
	output        o_allowJmp,
	output        o_wrReg,
	output        o_wrCC,
	output        o_isHLT
);

/*
 * TODO- desc.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Convenience wires.
wire [3:0] op;
	
////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Alias opcode for ease of writing logic.
assign op = i_opcode;

//------------------------------------------------------------------------------
// Drive control outputs.
assign o_allowJmp = ~op[3] & ~op[2] &  op[1] & ~op[0]; // 0010
assign o_wrReg    =  op[3] & ~op[2] &  op[1] & ~op[0]; // 1010
assign o_wrCC     =  op[3];                            // 1xxx
assign o_isHLT    = ~op[3] & ~op[2] &  op[1] &  op[0]; // 0011

endmodule
