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
 
////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Synchronized wires.
wire        synchRstnIn, synchRstnOut;
wire [2:0]  synchJtagIn, synchJtagOut;
wire        synchJtagTCK, synchJtagTMS, synchJtagTDI;

// Jtag Port wires.
wire        jtagTCK, jtagTMS, jtagTDI, jtagTDO;
wire        jtagMemWr;
wire [15:0] jtagMemData;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Syncrhonizers for ("slower") inputs.
RstSynch SYNCH_RSTN (
	// Reset line to synchronize.
	.i_rstIn(synchRstnIn),
	.o_rstOut(synchRstnOut),
	
	// Common signals.
	.i_clk(i_sysClk)
);
ClkSynch SYNCH_JTAG[2:0] (
	.A(synchJtagIn),
	.Y(synchJtagOut),
	.clk(i_sysClk),
	.rstn(synchRstnOut)
);

// JTAG Port- parses JTAG pins for commands/data.
JtagPort JTAG_PORT(
	// JTAG pin connections.
	.i_TCK(jtagTCK),
	.i_TMS(jtagTMS),
	.i_TDI(jtagTDI),
	.o_TDO(jtagTDO),
	
	// Current state of the uP.
	.i_isBooted(1'b1),          // TODO- implement
	.i_isPaused(i_smDoPause),   // TODO- implement
	
	// Runtime memory connection.
	.i_memDataIn(io_memData),   // TODO- implement
	.o_memAddr(o_memAddr),      // TODO- implement
	.o_memDataOut(jtagMemData),
	.o_memWr(jtagMemWr),
	.o_memEn(o_memEn),          // TODO- implement
	
	// Common signals.
	.i_rstn(synchRstnOut)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Syncrhonize ("slower") inputs.
assign synchRstnIn  = i_sysRstn;
assign synchJtagIn  = {i_jtagTCK, i_jtagTMS, i_jtagTDI};
assign synchJtagTCK = synchJtagOut[2];
assign synchJtagTMS = synchJtagOut[1];
assign synchJtagTDI = synchJtagOut[0];

//------------------------------------------------------------------------------
// Interpret/Control JTAG pinout.
assign jtagTCK   = synchJtagTCK;
assign jtagTMS   = synchJtagTMS;
assign jtagTDI   = synchJtagTDI;
assign o_jtagTDO = jtagTDO;

//------------------------------------------------------------------------------ 
// TODO- implement.
// TODO- assign o_memAddr    = 16'b0000000000000000;
//assign o_memWr      = 1'b0;
//assign o_memEn      = 1'b0;
//assign io_memData   = 16'bZZZZZZZZZZZZZZZZ;
assign o_spiMOSI    = 1'b0;
assign o_spiCLK     = 1'b0;
assign o_spiCSn     = 1'b1;
assign io_gpioPins  = 16'bZZZZZZZZZZZZZZZZ;
assign o_smIsBooted = 1'b0;
assign o_smIsPaused = 1'b0;

// TODO- implement.
assign o_memWr = jtagMemWr;
Tristate TRI[15:0] (
	.A(jtagMemData),
	.Y(io_memData),
	.S(jtagMemWr)
);
 
endmodule
 