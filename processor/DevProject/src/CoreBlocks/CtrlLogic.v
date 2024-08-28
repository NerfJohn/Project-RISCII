/*
 * CtrlLogic.v
 *
 * "Computed control signals based on a given instruction opcode"
 */
module CtrlLogic (
	// Input opcode connection.
	input  [3:0]  i_opcode,
	
	// Control outputs.
	output [2:0]  o_aluOp,
	output        o_altSrc1,
	output        o_altSel,
	output        o_useImm,
	output        o_allowImm,
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

// Control wires (based off common opcode combos).
wire       opX010;
	
////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Alias opcode for ease of writing logic.
assign op = i_opcode;

//------------------------------------------------------------------------------
// Compute common opcode combos.
assign opX010 = ~op[2] &  op[1] & ~op[0];

//------------------------------------------------------------------------------
// Drive control outputs.
assign o_aluOp    = op[2:0] & {3{op[3]}};              // 1bbb vs. ADD
assign o_altSrc1  = opX010;                            // LBI... + BRC
assign o_altSel   = opX010;                            // LBI... + BRC
assign o_useImm   = ~op[3] | (op[3] & opX010);         // 0xxx, LBI
assign o_allowImm =  op[3];                            // 1xxx
assign o_allowJmp = ~op[3] & opX010;                   // BRC
assign o_wrReg    =  op[3] |                           // 1xxx or...
                     op[2] &  op[0] |                  // LDR, JLR or...
						  ~op[1] &  op[0];                   // SWP
assign o_wrCC     =  op[3];                            // 1xxx
assign o_isHLT    = ~op[3] & ~op[2] &  op[1] &  op[0]; // HLT

endmodule
