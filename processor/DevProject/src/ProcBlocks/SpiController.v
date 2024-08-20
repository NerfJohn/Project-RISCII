/*
 * SpiController.v
 *
 * "Centralized circuit to control uP's SPI bus to the storage chip"
 */
module SpiController (
	// JTAG port connection.
	input         i_jtagTCK,
	input         i_jtagTDI,
	input         i_jtagSpiEn,
	
	// Selected/driven spi/storage bus.
	output        o_spiCLK,
	output        o_spiMOSI,
	output        o_spiCSn,
	
	// Common signals.
	input         i_rstn
);

/*
 * Controller determining which circuit is in control of the uP's storage bus.
 *
 * At present, only the JTAG can control the storage bus. As the uP is
 * developed, additional sources (and necessary muxing) will be added.
 *
 * Design Notes:
 * 1) This module implements logic needed to "silence" clock line when idle.
 * 2) Beware potential race condition between CLK and CSn on exiting session.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Enable register wires.
wire        startD, startQ;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Start register- used to ensure CLK vs CSn timing starts correctly.
DffSynch START (
	.D(startD),
	.Q(startQ),
	.clk(i_jtagTCK),
	.rstn(i_rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Handle start register.
assign startD = i_jtagSpiEn;

//------------------------------------------------------------------------------
// Drive storage bus outputs.
assign o_spiCLK  = i_jtagTCK & startQ;      // ensure 1st posedge is not counted
assign o_spiMOSI = i_jtagTDI;
assign o_spiCSn  = ~(i_jtagSpiEn | startQ); // active-low chip select


endmodule
