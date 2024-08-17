/*
 * RstSynch.v
 *
 * "Two stage synchronizer/metastability blocker explicitly for reset signals"
 */
module RstSynch (
	// Reset line to synchronize.
	input  i_rstIn,
	output o_rstOut,
	
	// Common signals.
	input  i_clk
);

/*
 * Clock synchronizer (and metastability blocker) for reset (low) signal.
 *
 * Module runs active low reset signal through 2 synchronous dffs to properly
 * synchronize/capture input. Designed specifically for active low resets such
 * that it passes the signal through in a way that can be reversed by the input.
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
    .D(1'b1),      // Defaults to "inactive" reset value
    .Q(stage0Q),
    .clk(i_clk),
    .rstn(stage0D)
);
DffSynch STAGE1 (
    .D(1'b1),      // Defaults to "inactive" reset value
    .Q(stage1Q),
    .clk(i_clk),
    .rstn(stage1D)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Create "pipeline" through each stage.
assign stage0D     = i_rstIn; // Reset triggers series of forced resets
assign stage1D     = stage0Q;
assign o_rstOut    = stage1Q;

endmodule
