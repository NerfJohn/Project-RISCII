// Arithmetic Logic Unit (ALU)- the epitome of computation.
module alu(src1, src2, op, result);

// I/Os.
input[2:0] op;
input[15:0] src1, src2;
output[15:0] result;

// Intermediate wires.
wire[7:0] upperLbiByte;
wire[15:0] monoResult, multiResult;

// Result wires (result contenders).
wire[15:0] rsltAnd, rsltOr, rsltXor, rsltShift, rsltAdder, rsltLbi;

////////////////////////////////////////////
// ALU Work Floor- Computational Hardware //
////////////////////////////////////////////

// Bitwise Operations- simple calculations.
assign rsltAnd = src1 & src2;
assign rsltOr = src1 | src2;
assign rsltXor = src1 ^ src2;

// Shift Operations- barrel shifter.
shifter_16b iSHIFT0 (.in(src1), .cnt(src2[3:0]), .op({op[0], ~op[0] | src2[4]}), .out(rsltShift));

// Arithmetic Operations- adder.
add_16b iADD0 (.src1(src1 ^ {16{op[0]}}), .src2(src2), .cin(op[0]), .sum(rsltAdder), .cout(/* NC */));

// Immediate Operation- simple muxing.
mux2 iMUXES0[7:0] (.A(src1[7:0]), .B({8{src2[7]}}), .sel(src2[8]), .out(upperLbiByte));
assign rsltLbi = {upperLbiByte, src2[7:0]};

///////////////////////////////////////////
// ALU Selection- Output Selected Result //
///////////////////////////////////////////

// Select between the circuits that cover one (mono) result.
mux4 iMUXES1[15:0] (.A(rsltLbi), .B(rsltXor), .C(rsltOr), .D(rsltAnd), .sel({op[2], op[0]}), .out(monoResult));

// Select between the circuits that cover two (multi) results.
mux2 iMUXES2[15:0] (.A(rsltShift), .B(rsltAdder), .sel(op[2]), .out(multiResult));

// Select the final result to return.
mux2 iMUXES3[15:0] (.A(monoResult), .B(multiResult), .sel(op[1]), .out(result));

endmodule