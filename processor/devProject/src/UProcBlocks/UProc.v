/*
 * UProc.v
 *
 * "Top level of Project RISCII's microprocessor design"
 */
module UProc (
    // SRAM chip connections.
	 output [15:0] uproc_sramAddr,
	 inout  [15:0] uproc_sramData,
	 output        uproc_sramWr,
	 output        uproc_sramEn,
	 
	 // EEPROM SPI chip connections.
	 output        uproc_spiSCK,
	 output        uproc_spiSDO,
	 input         uproc_spiSDI,
	 output        uproc_spiSCS,
	 
	 // JTAG port connections.
	 input         uproc_jtagTCK,
	 input         uproc_jtagTDI,
	 output        uproc_jtagTDO,
	 input         uproc_jtagTMS,

    // Common signals.
    input         uproc_clk,
    input         uproc_rstn
);

endmodule
