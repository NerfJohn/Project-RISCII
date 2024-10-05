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
	
	// SPI port connections.
	input         i_mapSpiMOSI,
	input         i_mapSpiEn,
	input         i_mapSpiHold,
	
	// uP State connections.
	input         i_smIsBooted,
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
 * Controller determining which circuit controls the uP's storage bus.
 *
 * Sources include bootloader, SPI peripheral, and JTAG port (based on uP states
 * BOOTING, RUNNING, and PAUSED respectively). Note that sources vary in terms
 * of clock source and "enable vs hold clock" controls.
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
wire        inRunMode;
wire        unpausedMOSI, unpausedEn;
wire        curCLK, curMOSI, curEn, curHold;

// Update output wires.
wire        enD, enQ, enClk;
wire        hldD, hldQ, hldClk;
wire        outD, outQ, outClk;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Update Output registers- clocl inverted counter race condition on ext. read.
DffSynch ENABLE (
	.D(enD),
	.Q(enQ),
	.clk(enClk),    // custom clock behavior
	.rstn(i_rstn)
);
DffSynch HOLD (
	.D(hldD),
	.Q(hldQ),
	.clk(hldClk),   // custom clock behavior
	.rstn(i_rstn)
);
DffSynch OUTPUT (
	.D(outD),
	.Q(outQ),
	.clk(outClk),   // custom clock behavior
	.rstn(i_rstn)
);
	
////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute controller/uP state based controls.
assign inRunMode = i_smIsBooted & ~i_smIsPaused;
Mux2 M0 (
	.A(i_jtagTCK),     // Paused? use JTAG clock
	.B(i_clk),         // No?     use system clock
	.S(i_smIsPaused),
	.Y(curCLK)
);
Mux2 M1 (
	.A(i_mapSpiMOSI),  // Booted? use SPI port data
	.B(i_bootSpiMOSI), // No?     use bootloader's write data
	.S(i_smIsBooted),
	.Y(unpausedMOSI)
);
Mux2 M2 (
	.A(i_jtagTDI),     // Paused? use JTAG write data
	.B(unpausedMOSI),  // No?     process unpaused MOSI sources
	.S(i_smIsPaused),
	.Y(curMOSI)
);
Mux2 M3 (
	.A(i_mapSpiEn),    // Booted? use SPI port's enable
	.B(i_bootSpiEn),   // No?     use bootloader's enable
	.S(i_smIsBooted),
	.Y(unpausedEn)
);
Mux2 M4 (
	.A(i_jtagSpiEn),   // Paused? use JTAG's enable
	.B(unpausedEn),    // No?     process unpaused En sources
	.S(i_smIsPaused),
	.Y(curEn)
);
Mux2 M5 (
	.A(i_mapSpiHold),  // Running? use SPI port hold
	.B(1'b0),          // No?      never hold clock
	.S(inRunMode),
	.Y(curHold)
);

//------------------------------------------------------------------------------
// Handle output registers.
assign enD       = curEn;
assign enClk     = ~curCLK; // toggle enable opposite of selected clock
assign hldD      = curHold;
assign hldClk    = ~curCLK; // toggle hold opposite of selected clock
assign outD      = curMOSI;
assign outClk    = ~i_clk;  // toggle output opposite of fastest clock

//------------------------------------------------------------------------------
// Drive storage bus outputs.
assign o_spiCLK  = curCLK & enQ & ~hldQ; // only drive clock as needed/wanted
assign o_spiMOSI = outQ;
assign o_spiCSn  = ~enQ;                 // active-low chip select


endmodule
