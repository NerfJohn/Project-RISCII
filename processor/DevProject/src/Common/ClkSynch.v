/*
 * ClkSynch.v
 *
 * "Two stage synchronizer/metastability blocker for external pins"
 */
module ClkSynch (
	// Serial line to synchronize.
	input  A,
	output Y,
	
	// Common signals.
	input  clk,
	input  rstn
);

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Dff Stage wires.
wire stage0D, stage0Q;
wire stage1D, stage1Q;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Dffs implementing two stages of syncrhonizer.
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

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Create "pipeline" through each stage.
assign stage0D     = A;
assign stage1D     = stage0Q;
assign Y           = stage1Q;

endmodule
