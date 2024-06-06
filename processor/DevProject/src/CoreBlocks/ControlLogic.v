/*
 * ControlLogic.v
 *
 * "Computational logic for core's control signals- encapsulates the mess"
 */
module ControlLogic (
	// Inputs used to determine signals.
	input  [3:0]  i_opcode,
	input         i_immFlag,
	input         i_arithFlag,
	input         i_shiftFlag,
	
	// Computed signals related to instruction(s) detection.
	output        o_isPSE,
	output        o_isSWP,
	output        o_isMemWriter,
	output        o_isMemInstr,
	
	// Computed signals related to register file.
	output        o_isSrcDst,
	output        o_isSrcVal,
	output        o_isFileEn,
	
	// Computed signals related to selecting signals.
	output        o_isUseImm,
	
	// Computed signals related to ALU.
	output [2:0]  o_isAluOp,
	output        o_isAluFlag
);

/*
 * TODO- desc. module
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Wires related to easing logic readability.
wire [3:0] op;

// Computed signals related to similar/shared logic.
wire opX010;

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// For ease, use shorter alias to refer to opcode bits.
assign op = i_opcode;

//------------------------------------------------------------------------------
// Compute shared/similar logic.
assign opX010 = ~op[2] &  op[1] & ~op[0];

//------------------------------------------------------------------------------
// Compute instruction(s) detection signals.
assign o_isPSE       = ~op[3] & ~op[2] &  op[1] &  op[0];
assign o_isSWP       = ~op[3] & ~op[2] & ~op[1] &  op[0];
assign o_isMemWriter = ~op[0];
assign o_isMemInstr  = ~op[3] & ~op[1] & (op[2] |  op[0]); // LDR, STR, SWP 

//------------------------------------------------------------------------------
// Compute register file control signals.
assign o_isSrcDst =  opX010;                             // LBI (and BRC)
assign o_isSrcVal = ~op[3] & ~op[1] & (op[2] ^  op[0]);  // STR, SWP
assign o_isFileEn =  op[3] | (op[0] & (op[2] | ~op[1])); // data, LDR, JLR, SWP

//------------------------------------------------------------------------------
// Compute selection signals.
assign o_isUseImm = ~op[3] | opX010 | i_immFlag; // controls, LBI, data w/ flag

//------------------------------------------------------------------------------
// Compute ALU signals.
assign o_isAluOp   =  op[2:0] & {3{op[3]}};
assign o_isAluFlag = (op[2] & i_arithFlag) | (~op[2] & i_shiftFlag); // SHR/LBI

endmodule
