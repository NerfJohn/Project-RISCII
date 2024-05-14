/*
 * DffSynch.v
 * 
 * "Simple, Synchronous D-Flip-Flop Register; Base of all memory blocks"
 */
module DffSynch (
    // Flop I/O.
    input      D,
    output reg Q,
    
    // Common signals.
    input      clk,
    input      rstn
);

/////////////////////////////////////////////////////////
// -- Synch Reset DFF Common Verilog Implementation -- //
/////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Core Logic: Update saved value on rising clk edge.
always @(posedge clk) begin
    Q <= D & rstn; // update output for input & reset signals
end

endmodule
