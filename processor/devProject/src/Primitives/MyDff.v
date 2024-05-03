/*
 * MyDff.sv
 * 
 * "Simple, Synchronous D-Flip-Flop Register; Base of all memory blocks"
 */
module MyDff (
    // Flop I/O.
    input      D,
    output reg Q,
    
    // Common signals.
    input      clk,
    input      rstn
);

// Core Logic: Update logic on rising clk edge.
always @(posedge clk) begin
    Q <= D & rstn; // update output for input & reset signals
end

endmodule
