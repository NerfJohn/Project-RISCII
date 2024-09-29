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
// TODO- larger sigs: i_sysClk, synchJtagTDI, pauseIsPausedQ, bootSmNowBooted,
//                    synchRstnOut
 
////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Synchronized wires.
wire        synchRstnIn, synchRstnOut;
wire [2:0]  synchJtagIn, synchJtagOut;
wire        synchJtagTCK, synchJtagTMS, synchJtagTDI;
wire        synchPauseIn, synchPauseOut;

// Jtag Port wires.
wire        jtagTCK, jtagTMS, jtagTDI, jtagTDO;
wire        jtagIsBooted, jtagIsPaused;
wire [15:0] jtagMemAddr, jtagMemDataOut;
wire        jtagMemWr, jtagMemEn;
wire        jtagSpiAccess, jtagScanAccess;
wire        jtagDoPause;
wire        jtagNotSpiTDO;

// Memory Controller wires.
wire [15:0] memJtagAddr, memJtagDataIn;
wire        memJtagWr, memJtagEn;
wire [15:0] memBootAddr, memBootDataIn;
wire        memBootEn;
wire [15:0] memCoreAddr, memCoreDataIn;
wire        memCoreWr;
wire        memSmIsPaused, memSmIsBooted;
wire [15:0] memMapReadData;
wire [15:0] memMemAddr;
wire        memMemRunWr, memMemRunEn, memMemMapWrEn, memMemMapRdEn;

// Storage Controller wires.
wire        spiJtagTCK, spiJtagTDI, spiJtagSpiEn;
wire        spiBootSpiMOSI, spiBootSpiEn;
wire        spiSmIsPaused;
wire        spiSpiCLK, spiSpiMOSI, spiSpiCSn;

// Boundary scan wires.
wire        scanShiftIn, scanShiftOut;
wire        scanDoShift;
wire [15:0] scanMemAddr;
wire        scanMemWr, scanMemEn;
wire        scanSpiMISO, scanSpiMOSI, scanSpiCLK, scanSpiCSn;
wire        scanSmDoPause, scanSmIsBooted, scanSmIsPaused;

// Pause Network wires.
wire        pauseStartPauseD, pauseStartPauseQ;
wire        pauseIsPausedD, pauseIsPausedQ;

// Mapped Register wires.
wire [13:0] mapMemAddr;
wire [15:0] mapMemDataOut;
wire        mapMemWrEn, mapMemRdEn;
wire        mapSmIsBooted, mapSmStartPause, mapSmNowPaused;
wire [15:0] mapReportSP;
wire [14:0] mapReportPC;
wire        mapReportHLT;
wire [3:0]  mapIntCode;
wire        mapIntEn, mapDoPause;
wire        mapDoReset;

// Bootloader wires.
wire        bootSpiMISO, bootSpiMOSI, bootSpiEn;
wire [15:0] bootMemAddr, bootMemDataOut;
wire        bootMemEn;
wire        bootSmNowBooted;

// Core wires.
wire [15:0] coreMemAddr, coreMemDataOut;
wire        coreMemWr;
wire        coreSmIsBooted, coreSmStartPause, coreSmNowPaused;
wire [3:0]  coreIntCode;
wire        coreIntEn;
wire [15:0] coreReportSP;
wire [14:0] coreReportPC;
wire        coreReportHLT;

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
ClkSynch SYNCH_PAUSE (
	.A(synchPauseIn),
	.Y(synchPauseOut),
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
	.i_clk(i_sysClk),
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
	
	// Bootloader connection.
	.i_bootMemAddr(memBootAddr),
	.i_bootDataIn(memBootDataIn),
	.i_bootMemEn(memBootEn),
	
	// Core connection.
	.i_coreMemAddr(memCoreAddr),
	.i_coreMemDataIn(memCoreDataIn),
	.i_coreMemWr(memCoreWr),
	
	// uP State connection.
	.i_smIsPaused(memSmIsPaused),
	.i_smIsBooted(memSmIsBooted),
	
	// Mapped read connection.
	.i_mapReadData(memMapReadData),
	
	// Selected/driven memory bus (contains tristated data line).
	.o_memAddr(memMemAddr),
	.o_memRunWr(memMemRunWr),
	.o_memRunEn(memMemRunEn),
	.o_memMapWrEn(memMemMapWrEn),
	.o_memMapRdEn(memMemMapRdEn),
	.io_memData(io_memData),       // inout- direct connect net
	
	// Common signals.
	.i_clk(i_sysClk),
	.i_rstn(synchRstnOut)
);

//------------------------------------------------------------------------------
// Storage Controller- handles assigning storage bus control.
SpiController SPI_CTRL (
	// JTAG port connection.
	.i_jtagTCK(spiJtagTCK),
	.i_jtagTDI(spiJtagTDI),
	.i_jtagSpiEn(spiJtagSpiEn),
	
	// Bootloader connections.
	.i_bootSpiMOSI(spiBootSpiMOSI),
	.i_bootSpiEn(spiBootSpiEn),
	
	// uP State connections.
	.i_smIsPaused(spiSmIsPaused),
	
	// Selected/driven spi/storage bus.
	.o_spiCLK(spiSpiCLK),
	.o_spiMOSI(spiSpiMOSI),
	.o_spiCSn(spiSpiCSn),
	
	// Common signals.
	.i_clk(i_sysClk),
	.i_rstn(synchRstnOut)
);

//------------------------------------------------------------------------------
// Boundary Scan- provides JTAG read access to most external pins.
BoundaryScan BOUNDARY_SCAN (
	// Shift connections.
	.i_shiftIn(scanShiftIn),
	.o_shiftOut(scanShiftOut),
	
	// Shift control.
	.i_doShift(scanDoShift),
	
	// Read connections.
	.i_memAddr(scanMemAddr),
	.i_memData(io_memData),        // inout- direct connect net
	.i_memWr(scanMemWr),
	.i_memEn(scanMemEn),
	.i_spiMISO(scanSpiMISO),
	.i_spiMOSI(scanSpiMOSI),
	.i_spiCLK(scanSpiCLK),
	.i_spiCSn(scanSpiCSn),
	.i_gpioPins(io_gpioPins),      // inout- direct connect net
	.i_smDoPause(scanSmDoPause),
	.i_smIsBooted(scanSmIsBooted),
	.i_smIsPaused(scanSmIsPaused),
	
	// Common signals.
	.i_clk(synchJtagTCK),
	.i_rstn(synchRstnOut)
);

//------------------------------------------------------------------------------
// Pause Network Registers- directs "traffic" of pausing uP.
DffSynch PAUSE_START (
	.D(pauseStartPauseD),
	.Q(pauseStartPauseQ),
	.clk(i_sysClk),
	.rstn(synchRstnOut)
);
DffSynch PAUSE_STATE (
	.D(pauseIsPausedD),
	.Q(pauseIsPausedQ),
	.clk(i_sysClk),
	.rstn(synchRstnOut)
);

//------------------------------------------------------------------------------
// Mapped Registers- memory mapped devices available to uP core.
MappedRegisters MAPPED_REGS (
	// Memory Bus connections.
	.i_memAddr(mapMemAddr),
	.i_memDataIn(io_memData),      // inout- direct connect net
	.i_memWrEn(mapMemWrEn),
	.i_memRdEn(mapMemRdEn),
	.o_memDataOut(mapMemDataOut),
	
	// State machine connections.
	.i_smIsBooted(mapSmIsBooted),
	.i_smStartPause(mapSmStartPause),
	.o_smNowPaused(mapSmNowPaused),
	
	// Reported Info connections.
	.i_reportSP(mapReportSP),
	.i_reportPC(mapReportPC),
	.i_reportHLT(mapReportHLT),
	
	// Output Control connections.
	.o_doPause(mapDoPause),
	.o_intCode(mapIntCode),
	.o_intEn(mapIntEn),
	
	// Output reset connection.
	.o_doReset(mapDoReset),
	
	// Driven GPIO connections.
	.io_gpioPins(io_gpioPins),     // inout- direct connect net
	
	// Common signals.
	.i_clk(i_sysClk),
	.i_rstn(synchRstnOut)
);

//------------------------------------------------------------------------------
// Bootloader- reads binary image from storage, copying values to runtime chip.
BootImage BOOT_IMAGE (
	// Storage chip connections.
	.i_spiMISO(bootSpiMISO),
	.o_spiMOSI(bootSpiMOSI),
	.o_spiEn(bootSpiEn),

	// Runtime chip connections.
	.o_memAddr(bootMemAddr),
	.o_memDataOut(bootMemDataOut),
	.o_memEn(bootMemEn),
	
	// Asserted state signal.
	.o_smNowBooted(bootSmNowBooted),
	
	// Common signals.
	.i_clk(i_sysClk),
	.i_rstn(synchRstnOut)
);

//------------------------------------------------------------------------------
// Processing Core- executes software program and computations.
Core CORE (
	// Memory port connections.
	.i_memDataIn(io_memData),          // inout- direct connect net
	.o_memAddr(coreMemAddr),
	.o_memDataOut(coreMemDataOut),
	.o_memWr(coreMemWr),
	
	// uP State connections.
	.i_smIsBooted(coreSmIsBooted),
	.i_smStartPause(coreSmStartPause),
	.o_smNowPaused(coreSmNowPaused),
	
	// Interrupt connections.
	.i_intCode(coreIntCode),
	.i_intEn(coreIntEn),
	
	// Reported control connections.
	.o_reportSP(coreReportSP),
	.o_reportPC(coreReportPC),
	.o_reportHLT(coreReportHLT),
	
	// Common signals.
	.i_clk(i_sysClk),
	.i_rstn(synchRstnOut)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Syncrhonize ("slower") inputs.
assign synchRstnIn  = i_sysRstn & ~mapDoReset;           // External + Internal
assign synchJtagIn  = {i_jtagTCK, i_jtagTMS, i_jtagTDI}; // Group synch for ease
assign synchJtagTCK = synchJtagOut[2];
assign synchJtagTMS = synchJtagOut[1];
assign synchJtagTDI = synchJtagOut[0];
assign synchPauseIn = i_smDoPause;

//------------------------------------------------------------------------------
// Handle JTAG inputs.
assign jtagTCK      = synchJtagTCK;
assign jtagTMS      = synchJtagTMS;
assign jtagTDI      = synchJtagTDI;
assign jtagIsBooted = bootSmNowBooted;
assign jtagIsPaused = pauseIsPausedQ;

//------------------------------------------------------------------------------
// Handle Memory Controller inputs.
assign memJtagAddr    = jtagMemAddr;
assign memJtagDataIn  = jtagMemDataOut;
assign memJtagWr      = jtagMemWr;
assign memJtagEn      = jtagMemEn;
assign memBootAddr    = bootMemAddr;
assign memBootDataIn  = bootMemDataOut;
assign memBootEn      = bootMemEn;
assign memCoreAddr    = coreMemAddr;
assign memCoreDataIn  = coreMemDataOut;
assign memCoreWr      = coreMemWr;
assign memSmIsPaused  = pauseIsPausedQ;
assign memSmIsBooted  = bootSmNowBooted;
assign memMapReadData = mapMemDataOut;

//------------------------------------------------------------------------------
// Handle Storage Controller inputs.
assign spiJtagTCK     = synchJtagTCK;
assign spiJtagTDI     = synchJtagTDI;
assign spiJtagSpiEn   = jtagSpiAccess;
assign spiBootSpiMOSI = bootSpiMOSI; 
assign spiBootSpiEn   = bootSpiEn;
assign spiSmIsPaused  = pauseIsPausedQ;

//------------------------------------------------------------------------------
// Handle Boundary Scan inputs.
assign scanShiftIn    = synchJtagTDI;
assign scanDoShift    = jtagScanAccess;
assign scanMemAddr    = o_memAddr;
assign scanMemWr      = o_memWr;
assign scanMemEn      = o_memEn;
assign scanSpiMISO    = i_spiMISO;
assign scanSpiMOSI    = o_spiMOSI;
assign scanSpiCLK     = o_spiCLK;
assign scanSpiCSn     = o_spiCSn;
assign scanSmDoPause  = i_smDoPause;
assign scanSmIsBooted = o_smIsBooted;
assign scanSmIsPaused = o_smIsPaused;

//------------------------------------------------------------------------------
// Handle Pause Network inputs.
assign pauseStartPauseD = synchPauseOut       // src: "pause" pin
                          | jtagDoPause       // src: JTAG's pause request
								  | mapDoPause;       // src: CCTRL pause bit (ie Core)
assign pauseIsPausedD   = pauseStartPauseQ    // "unpause" within 1 cycle
                          & coreSmNowPaused   // core must be locally paused
								  & bootSmNowBooted   // formally pause when booted
								  & mapSmNowPaused;   // map regs must be locally paused

//------------------------------------------------------------------------------
// Handle Mapped Registers inputs.
assign mapMemAddr      = memMemAddr[13:0];
assign mapMemWrEn      = memMemMapWrEn;
assign mapMemRdEn      = memMemMapRdEn;
assign mapSmIsBooted   = bootSmNowBooted;
assign mapSmStartPause = pauseStartPauseQ;
assign mapReportSP     = coreReportSP;
assign mapReportPC     = coreReportPC;
assign mapReportHLT    = coreReportHLT;

//------------------------------------------------------------------------------
// Handle Bootloader inputs.
assign bootSpiMISO = i_spiMISO;

//------------------------------------------------------------------------------
// Handle core inputs.
assign coreSmIsBooted   = bootSmNowBooted;
assign coreSmStartPause = pauseStartPauseQ;
assign coreIntCode      = mapIntCode;
assign coreIntEn        = mapIntEn;

//------------------------------------------------------------------------------
// Drive external memory bus (to runtime chip).
assign o_memAddr = memMemAddr;
assign o_memWr   = memMemRunWr;
assign o_memEn   = memMemRunEn;

//------------------------------------------------------------------------------
// Drive external spi bus (to storage chip).
assign o_spiCLK  = spiSpiCLK;
assign o_spiMOSI = spiSpiMOSI;
assign o_spiCSn  = spiSpiCSn;

//------------------------------------------------------------------------------
// Drive JTAG output pin.
Mux2 M0 (
	.A(scanShiftOut),   // Accessing Scan? Relay Scan response
	.B(jtagTDO),        // No?             Relay JTAG port shift bit
	.S(jtagScanAccess),  
	.Y(jtagNotSpiTDO)
);
Mux2 M1 (
	.A(i_spiMISO),      // Accessing SPI? Relay SPI response
	.B(jtagNotSpiTDO),  // No?            Further process bit to send...
	.S(jtagSpiAccess),
	.Y(o_jtagTDO)
);

//------------------------------------------------------------------------------
// Drive state machine indicator pins.
assign o_smIsBooted = bootSmNowBooted;
assign o_smIsPaused = pauseIsPausedQ;
 
endmodule
 