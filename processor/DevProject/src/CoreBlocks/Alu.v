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
	input  [2:0]  i_opCode,
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

// Adder computation wires.
wire [15:0] adderA, adderB, adderS;
wire        adderI, adderO;
wire [15:0] adderResult;

// Bitwise computation wires.
wire [15:0] andResult, orResult, xorResult;

// Barrel shift computation wires.
wire [15:0] shftOpValue, shftResult;
wire [3:0]  shftOpCount;
wire [1:0]  shftOpCode;
wire [15:0] shiftResult;

// Selection net wires.
wire [15:0] select0XX, select1XX;
wire [15:0] finResult;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Adder- arithmetic unit used for addition/subtraction computations.
AddCout16 ADDER (
	.A(adderA),
	.B(adderB),
	.I(adderI),
	.O(adderO),
	.S(adderS)
);

//------------------------------------------------------------------------------
// Barrel Shifter- shift unit used for various types of 16-bit shifts.
BarrelShift BARREL_SHIFT (
	// Operand connections.
	.i_opValue(shftOpValue),
	.i_opCount(shftOpCount),
	
	// Operation connection.
	.i_opCode(shftOpCode),
	
	// Computed connection.
	.o_result(shftResult)
);
	
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
// Compute "arithmetic" result (ie ADD/SUB).
assign adderA      = i_srcA ^ {16{i_opCode[0]}}; // SUB's "invert"
assign adderB      = i_srcB;
assign adderI      = i_opCode[0];                // SUB's "+1" for inverting
assign adderResult = adderS;

//------------------------------------------------------------------------------
// Compute bitwise results (ie AND/ORR/XOR).
assign andResult = i_srcA & i_srcB;
assign orResult  = i_srcA | i_srcB;
assign xorResult = i_srcA ^ i_srcB;

//------------------------------------------------------------------------------
// Compute shift result (ie SHL/SHR).
assign shftOpValue = i_srcA;
assign shftOpCount = i_srcB[3:0];
assign shftOpCode  = {i_opCode[0], ~(i_opCode[0] & i_opSel)};
assign shiftResult = shftResult;

//------------------------------------------------------------------------------
// Drive result selection.
Mux4 M1[15:0] (
	.C(adderResult),   // ADD (000)? Select adder result
	.D(adderResult),   // SUB (001)? Select adder result
	.E(setResult),     // LBI (010)? Select set result
	.F(xorResult),     // XOR (011)? Select XOR result
	.S(i_opCode[1:0]),
	.Y(select0XX)
);
Mux4 M2[15:0] (
	.C(shiftResult),   // SHL (100)? Select shift result
	.D(shiftResult),   // SHR (101)? Select shift result
	.E(orResult),      // ORR (110)? Select OR result
	.F(andResult),     // AND (111)? Select AND result
	.S(i_opCode[1:0]),
	.Y(select1XX)
);
Mux2 M3[15:0] (
	.A(select1XX),     // Select 1XX? Process 1XX results
	.B(select0XX),     // Select 0XX? Process 0XX results
	.S(i_opCode[2]),
	.Y(finResult)
);
assign o_result = finResult;

//------------------------------------------------------------------------------
// Drive condition code computation.
assign o_ccodes = {finResult[15],  // (n)egative flag
                   ~(|finResult),  // (z)ero flag
						 ~finResult[15], // (p)ositive flag
						 adderO};        // (c)arry-out flag (explicitly for SUB)

endmodule
