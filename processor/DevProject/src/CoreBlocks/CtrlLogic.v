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
	output        o_altSrc2,
	output        o_altSel,
	output        o_useImm,
	output        o_allowImm,
	output        o_useJmp,
	output        o_allowJmp,
	output        o_wrReg,
	output        o_wrCC,
	output        o_wrMem,
	output        o_isMemInstr,
	output        o_isSWP,
	output        o_isJPR,
	output        o_isHLT
);

/*
 * Control signals needed to execute given opcode (based on opcode).
 *
 * Computes control signals based solely on opcode. Controls vary in terms of
 * "general net to capture a few instructions" and "must capture explicit set".
 * Note that some instructions require additional control NOT based on opcode.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Convenience wires.
wire [3:0] op;

// Control wires (based off common opcode combos).
wire       opX010, opX111;
	
////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Alias opcode for ease of writing logic.
assign op = i_opcode;

//------------------------------------------------------------------------------
// Compute common opcode combos.
assign opX010 = ~op[2] &  op[1] & ~op[0];
assign opX111 =  op[2] &  op[1] &  op[0];

//------------------------------------------------------------------------------
// Drive control outputs.
assign o_aluOp      = op[2:0] & {3{op[3]}};              // 1bbb vs. ADD
assign o_altSrc1    = opX010;                            // x010 (LBI)
assign o_altSrc2    = ~op[3];                            // 0xxx (STR, SWP)
assign o_altSel     = opX010;                            // x010 (LBI)
assign o_useImm     = ~op[3] | (op[3] & opX010);         // 0xxx, LBI
assign o_allowImm   =  op[3];                            // 1xxx
assign o_useJmp     = ~op[3] & opX111;                   // JLR
assign o_allowJmp   = ~op[3] & opX010;                   // BRC
assign o_wrReg      =  op[3] | opX111;                   // 1xxx, JLR
assign o_wrCC       =  op[3];                            // 1xxx
assign o_wrMem      = ~op[0];                            // xxx0 (STR)
assign o_isMemInstr = ~op[3] & ~op[1] & (op[2] | op[0]); // LDR, STR, SWP
assign o_isSWP      = ~op[3] & ~op[2] & ~op[1] &  op[0]; // SWP
assign o_isJPR      = ~op[3] &  op[2] &  op[1] & ~op[0]; // JPR
assign o_isHLT      = ~op[3] & ~op[2] &  op[1] &  op[0]; // HLT

endmodule
