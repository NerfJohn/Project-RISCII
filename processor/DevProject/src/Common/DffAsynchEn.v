/*
 * DffAsynchEn.v
 * 
 * "Asynchronous DFF with enable signal (i.e. alternate form of Asynch DFF)"
 */
module DffAsynchEn (
    // Flop I/O.
    input  D,
    output Q,
    
    // Control signal.
    input  S,
    
    // Common signals.
    input  clk,
    input  rstn
);

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Mux output choosing selected DFF input.
wire newD;

// Internal DFF wires.
wire regD, regQ;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Typical DFF (arguable not primitive, but close enough).
DffAsynch CORE_DFF (
    .D(regD),
    .Q(regQ),
    .clk(clk),
    .rstn(rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Select the next input to be saved (ie if DFF accepts updated info).
Mux2 M0 (
    .A(D),
    .B(regQ),
    .S(S),
    .Y(newD)
);

//------------------------------------------------------------------------------
// Update DFF with selected input.
assign regD = newD;

//------------------------------------------------------------------------------
// Output DFF value.
assign Q = regQ;

endmodule
