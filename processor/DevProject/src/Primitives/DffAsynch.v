/*
 * DffAsynch.v
 * 
 * "Asynchronous DFF for unique cases where synchronous won't cut it"
 */
module DffAsynch (
    // Flop I/O.
    input      D,
    output reg Q,
    
    // Common signals.
    input      clk,
    input      rstn
);

////////////////////////////////////////////////////////////////////////////////
// -- PRIMITIVE_DFF_ASYNCH Wires/Blocks/Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Core Logic: Update logic on rising clk edge OR falling reset edge.
always @(posedge clk, negedge rstn) begin
    if (~rstn)  Q <= 1'b0;     // Asynchronous, active-low reset
    else        Q <= D;        // Synchronous update
end

endmodule
