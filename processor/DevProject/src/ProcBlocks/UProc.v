/*
 * UProc.v
 *
 * "Top level of the RISCII Microprocessor Design"
 */
module UProc (
	// Runtime Chip (Parallel Port SRAM) Connections.
	output [15:0] o_memAddr,
	output        o_memWr,
	output        o_memEn,
	inout  [15:0] io_memData,
	
	// Storage Chip (SPI Port EEPROM) Connections.
	input         i_spiMISO,
	output        o_spiMOSI,
	output        o_spiCLK,
	output        o_spiCSn,
	
	// GPIO Pin Connections.
	inout  [15:0] io_gpioPins,
	
	// JTAG(esque) Port Connections.
	input         i_jtagTCK,
	input         i_jtagTMS,
	input         i_jtagTDI,
	output        o_jtagTDO,
	
	// State Machine Connections.
	input         i_smDoPause,
	output        o_smIsBooted,
	output        o_smIsPaused,
	
	// Common System Connections.
	input         i_sysClk,
	input         i_sysRstn
);
 
/*
 * TODO- desc.
 */
 
// TODO- implement.
assign o_memAddr    = 16'b0000000000000000;
assign o_memWr      = 1'b0;
assign o_memEn      = 1'b0;
assign io_memData   = 16'bZZZZZZZZZZZZZZZZ;
assign o_spiMOSI    = 1'b0;
assign o_spiCLK     = 1'b0;
assign o_spiCSn     = 1'b1;
assign io_gpioPins  = 16'bZZZZZZZZZZZZZZZZ;
assign o_jtagTDO    = 1'b0;
assign o_smIsBooted = 1'b0;
assign o_smIsPaused = 1'b0;
 
endmodule
 