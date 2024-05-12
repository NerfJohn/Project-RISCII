/*
 * DffEn.v
 * 
 * "Synchronous DFF with enable signal (i.e. alternate form of DFF)"
 */
module DffEn (
    // Flop I/O.
    input  D,
    output Q,
    
    // Control signal.
    input  S,
    
    // Common signals.
    input  clk,
    input  rstn
);

// Wire selecting input for DFF.
wire newD;

// Mux used to select if DFF should update.
Mux2 M0 (
    .A(D),
    .B(Q),
    .S(S),
    .Y(newD)
);

// Typical DFF (arguable not primitive, but close enough).
MyDff CORE_DFF (
    .D(newD),
    .Q(Q),
    .clk(clk),
    .rstn(rstn)
);

endmodule
