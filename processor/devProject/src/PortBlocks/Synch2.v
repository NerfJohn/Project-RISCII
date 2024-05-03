/*
 * Synch2.sv
 * 
 * "Simple 2 cycle synchronizer to synch signals and mitigate metastability"
 */
module Synch2 (
    // Basic I/O.
    input  A,
    output Y,
    
    // Common signals.
    input  clk,
    input  rstn
);

/////////////////////////
// -- Signals/Wires -- //
/////////////////////////

// Dff Stage wires.
wire stage0D, stage0Q;
wire stage1D, stage1Q;

////////////////////////////
// -- Blocks/Instances -- //
////////////////////////////

// Two Dffs implementing two stages of synchronizer.
MyDff STAGE0 (
    .D(stage0D),
    .Q(stage0Q),
    .clk(clk),
    .rstn(rstn)
);
MyDff STAGE1 (
    .D(stage1D),
    .Q(stage1Q),
    .clk(clk),
    .rstn(rstn)
);

//////////////////////////
// -- Connects/Logic -- //
//////////////////////////

// Create "pipeline" through each stage.
assign stage0D = A;
assign stage1D = stage0Q;
assign Y = stage1Q;

endmodule
