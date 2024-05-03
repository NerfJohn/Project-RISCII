/*
 * DffEn.sv
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

// Clock gated DFF (arguable not primitive, but close enough).
MyDff CORE_DFF (
    .D(D),
    .Q(Q),
    .clk(clk & S),
    .rstn(rstn)
);

endmodule
