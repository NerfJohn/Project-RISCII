/*
 * Core.v
 *
 * "Core processing unit of the uP- executioner of program code and data"
 */
module Core (
	// Memory port connections.
	//input  [15:0] i_memDataIn,
	output [15:0] o_memAddr,
	output [15:0] o_memDataOut,
	output        o_memWr,
	
	// uP State connections.
	//input         i_smIsBooted,
	input         i_smStartPause,
	output        o_smNowPaused,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * TODO- desc.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Pause wires.
wire        pauseD, pauseQ;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Pause Output- local PAUSED state of core (and confirmation of pause state).
DffSynch LOCAL_PAUSE (
	.D(pauseD),
	.Q(pauseQ),
	.clk(i_clk),
	.rstn(i_rstn)
);
	
////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Handle pause inputs.
assign pauseD = i_smStartPause;

//------------------------------------------------------------------------------
// TODO- implement.
assign o_memAddr    = 16'h000A;
assign o_memDataOut = 16'hdada;
assign o_memWr      = 1'b1;

//------------------------------------------------------------------------------
// Drive state machine outputs.
assign o_smNowPaused = pauseQ;

endmodule
