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
wire        jtagIsBooted, jtagIsPaused;
wire [15:0] jtagMemAddr, jtagMemDataOut;
wire        jtagMemWr, jtagMemEn;
wire        jtagSpiAccess, jtagScanAccess;
wire        jtagDoPause;

// Memory Controller wires.
wire [15:0] memJtagAddr, memJtagDataIn;
wire        memJtagWr, memJtagEn;
wire [15:0] memMemAddr;
wire        memMemWr, memMemEn;

// Storage Controller wires.
wire        spiJtagTCK, spiJtagTDI, spiJtagSpiEn;
wire        spiSpiCLK, spiSpiMOSI, spiSpiCSn;

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

//------------------------------------------------------------------------------
// JTAG Port- parses JTAG pins for commands/data.
JtagPort JTAG_PORT(
	// JTAG pin connections.
	.i_TCK(jtagTCK),
	.i_TMS(jtagTMS),
	.i_TDI(jtagTDI),
	.o_TDO(jtagTDO),
	
	// Current state of the uP.
	.i_isBooted(jtagIsBooted),
	.i_isPaused(jtagIsPaused),
	
	// Runtime memory connection.
	.i_memDataIn(io_memData),      // inout- direct connect net
	.o_memAddr(jtagMemAddr),
	.o_memDataOut(jtagMemDataOut),
	.o_memWr(jtagMemWr),
	.o_memEn(jtagMemEn),
	
	// Access enable connections.
	.o_spiAccess(jtagSpiAccess),
	.o_scanAccess(jtagScanAccess),
	
	// Pause sequence trigger.
	.o_doPause(jtagDoPause),
	
	// Common signals.
	.i_rstn(synchRstnOut)
);

//------------------------------------------------------------------------------
// Memory Controller- handles assigning memory bus control/tristating.
MemController MEM_CTRL(
	// JTAG port connection.
	.i_jtagAddr(memJtagAddr),
	.i_jtagDataIn(memJtagDataIn),
	.i_jtagWr(memJtagWr),
	.i_jtagEn(memJtagEn),
	
	// Selected/driven memory bus (contains tristated data line).
	.o_memAddr(memMemAddr),
	.o_memWr(memMemWr),
	.o_memEn(memMemEn),
	.io_memData(io_memData)       // inout- direct connect net
);

//------------------------------------------------------------------------------
// Storage Controller- handles assigning storage bus control.
SpiController SPI_CTRL (
	// JTAG port connection.
	.i_jtagTCK(spiJtagTCK),
	.i_jtagTDI(spiJtagTDI),
	.i_jtagSpiEn(spiJtagSpiEn),
	
	// Selected/driven spi/storage bus.
	.o_spiCLK(spiSpiCLK),
	.o_spiMOSI(spiSpiMOSI),
	.o_spiCSn(spiSpiCSn),
	
	// Common signals.
	.i_rstn(synchRstnOut)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Syncrhonize ("slower") inputs.
assign synchRstnIn  = i_sysRstn;
assign synchJtagIn  = {i_jtagTCK, i_jtagTMS, i_jtagTDI}; // Group synch for ease
assign synchJtagTCK = synchJtagOut[2];
assign synchJtagTMS = synchJtagOut[1];
assign synchJtagTDI = synchJtagOut[0];

//------------------------------------------------------------------------------
// Handle JTAG inputs.
assign jtagTCK   = synchJtagTCK;
assign jtagTMS   = synchJtagTMS;
assign jtagTDI   = synchJtagTDI;
assign jtagIsBooted = 1'b1;                      // TODO- implement
assign jtagIsPaused = i_smDoPause | jtagDoPause; // TODO- implement

//------------------------------------------------------------------------------
// Handle Memory Controller inputs.
assign memJtagAddr   = jtagMemAddr;
assign memJtagDataIn = jtagMemDataOut;
assign memJtagWr     = jtagMemWr;
assign memJtagEn     = jtagMemEn;

//------------------------------------------------------------------------------
// Handle Storage Controller inputs.
assign spiJtagTCK   = synchJtagTCK;
assign spiJtagTDI   = synchJtagTDI;
assign spiJtagSpiEn = jtagSpiAccess;

//------------------------------------------------------------------------------
// Drive external memory bus (to runtime chip).
assign o_memAddr = memMemAddr;
assign o_memWr = memMemWr;
assign o_memEn = memMemEn;

//------------------------------------------------------------------------------
// Drive external spi bus (to storage chip).
assign o_spiCLK  = spiSpiCLK;
assign o_spiMOSI = spiSpiMOSI;
assign o_spiCSn  = spiSpiCSn;

//------------------------------------------------------------------------------
// Drive JTAG output pin.
Mux2 M0 (
	.A(i_spiMISO),     // Accessing SPI? Relay SPI response bit
	.B(jtagTDO),       // No?            Relay JTAG port shift bit
	.S(jtagSpiAccess),
	.Y(o_jtagTDO)
);

//------------------------------------------------------------------------------ 
// TODO- implement.
assign io_gpioPins  = 16'bZZZZZZZZZZZZZZZZ;
assign o_smIsBooted = spiSpiMOSI;
assign o_smIsPaused = spiSpiCLK;
 
endmodule
 