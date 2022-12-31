// 2 signal multiplexer- common component in signal processing.
module mux2 (A, B, sel, out);

// I/Os.
input A, B, sel;
output out;

// Mux logic: (sel) ? A : B;
assign out = (sel & A) | (~sel & B);

endmodule