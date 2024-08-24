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
	
	// Bootloader connections.
	input         i_bootSpiMOSI,
	input         i_bootSpiEn,
	
	// uP State connections.
	input         i_smIsPaused,
	
	// Selected/driven spi/storage bus.
	output        o_spiCLK,
	output        o_spiMOSI,
	output        o_spiCSn,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * Controller determining which circuit is in control of the uP's storage bus.
 *
 * At present, the two potential controllers are the internal bootloader circuit
 * and JTAG interface (chosen based if uP has been booted). Note that each uses
 * separate clocks (JTAG uses TCK, the bootloader uses the system clock).
 *
 * Design Notes:
 * 1) This module implements logic needed to "silence" clock line when idle.
 * 2) This module uses a mux clock to drive a DFF on FALLING edge.
 * 3) While JTAG uses TCK for clocking, other sources use system clock.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Computed controls (based on potential controllers).
wire        curCLK, curMOSI, curEn;

// Update output wires.
wire        enD, enQ, enClk;
wire        outD, outQ, outClk;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Update Output registers- clocl inverted counter race condition on ext. read.
DffSynch ENABLE (
	.D(enD),
	.Q(enQ),
	.clk(enClk),   // custom clock behavior
	.rstn(i_rstn)
);
DffSynch OUTPUT (
	.D(outD),
	.Q(outQ),
	.clk(outClk),  // custom clock behavior
	.rstn(i_rstn)
);
	
////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute controller/uP state based controls.
Mux2 M0 (
	.A(i_jtagTCK),     // Paused? use JTAG clock
	.B(i_clk),         // No?     use system clock
	.S(i_smIsPaused),
	.Y(curCLK)
);
Mux2 M1 (
	.A(i_jtagTDI),     // Paused? use JTAG write data
	.B(i_bootSpiMOSI), // No?     use bootloader's write data
	.S(i_smIsPaused),
	.Y(curMOSI)
);
Mux2 M2 (
	.A(i_jtagSpiEn),   // Paused? use JTAG's enable
	.B(i_bootSpiEn),   // No?     use bootloader's enable
	.S(i_smIsPaused),
	.Y(curEn)
);

//------------------------------------------------------------------------------
// Handle start register.
assign enD       = curEn;
assign enClk     = ~curCLK; // toggle enable opposite of selected clock
assign outD      = curMOSI;
assign outClk    = ~i_clk;  // toggle output opposite of fastest clock

//------------------------------------------------------------------------------
// Drive storage bus outputs.
assign o_spiCLK  = curCLK & enQ;   // only drive clock as needed
assign o_spiMOSI = outQ;
assign o_spiCSn  = ~enQ;           // active-low chip select


endmodule
