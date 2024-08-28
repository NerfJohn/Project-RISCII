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
// Drive result selection.
Mux4 M1[15:0] (
	.C(adderResult),   // ADD (000)? Select adder result
	.D(adderResult),   // SUB (001)? Select adder result
	.E(setResult),     // LBI (010)? Select set result
	.F(16'b0),         // TODO
	.S(i_opCode[1:0]),
	.Y(select0XX)
);
Mux4 M2[15:0] (
	.C(16'b0),         // TODO
	.D(16'b0),         // TODO
	.E(16'b0),         // TODO
	.F(16'b0),         // TODO
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
