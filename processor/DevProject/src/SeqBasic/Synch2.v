/*
 * Synch2.v
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

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Dff Stage wires.
wire stage0D, stage0Q;
wire stage1D, stage1Q;

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------
// Two Dffs implementing two stages of synchronizer.
DffSynch STAGE0 (
    .D(stage0D),
    .Q(stage0Q),
    .clk(clk),
    .rstn(rstn)
);
DffSynch STAGE1 (
    .D(stage1D),
    .Q(stage1Q),
    .clk(clk),
    .rstn(rstn)
);

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Create "pipeline" through each stage.
assign stage0D = A;
assign stage1D = stage0Q;
assign Y = stage1Q;

endmodule
