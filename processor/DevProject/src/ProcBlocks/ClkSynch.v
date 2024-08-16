/*
 * ClkSynch.v
 *
 * "Two stage synchronizer/metastability blocker for external pins"
 */
module ClkSynch (
	// Serial line to synchronize.
	input  i_asynchLine,
	output o_synchLine,
	
	// Common signals.
	input  i_clk,
	input  i_rstn
);

/*
 * Clock synchronizer used to synchronize line with a clock signal.
 *
 * Moreover, synchronizer acts as a means of containing metastability. Given
 * line must be directional and be able to tolerate the 2 clk posedges needed to
 * synchronize the line. Reset is synchronous for this module.
 */

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
    .clk(i_clk),
    .rstn(i_rstn)
);
DffSynch STAGE1 (
    .D(stage1D),
    .Q(stage1Q),
    .clk(i_clk),
    .rstn(i_rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Create "pipeline" through each stage.
assign stage0D     = i_asynchLine;
assign stage1D     = stage0Q;
assign o_synchLine = stage1Q;

endmodule
