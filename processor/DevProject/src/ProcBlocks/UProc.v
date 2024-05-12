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
    input         uproc_rstn,
     
    // TODO- test signals for development. TO DELETE!!!
    output [15:0] test_word0,
    output [15:0] test_word1
);

/*
 * Microprocessor ("UProc") Top-Level Design:
 *
 * TODO- summary of module + design notes. Keep list of modules- order later.
 *
 * module name          | desc.
 * ---------------------+------
 * JTAG Synch           | Synchronizer for JTAG pins (ie not boundary scanned)
 * Memory Controller    | Controller for SRAM/Mapped Memory Bus
 * JTAG Port            | Port/Controller for handling JTAG signals
 */

/////////////////////////
// -- Signals/Wires -- //
/////////////////////////

// Internal Global Signals.
wire[15:0] gbl_memData;

// Synchronized JTAG wires.
wire[2:0] jtagSynchA,   jtagSynchY;
wire      jtagSynchTCK, jtagSynchTDI, jtagSynchTMS;

// JTAG port/controller wires.
wire[15:0] jtagSramAddr;
wire       jtagSramWr,  jtagSramEn;
wire       jtagEnScan,  jtagEnSPI;
wire       jtagDoPause;
wire       jtagBaseTDO;

// Memory Bus Controller wires.
wire[15:0] memBusAddr;
wire       memBusWr;
wire       memBusRamEn, memBusMapEn;

// Storage Controller wires.
wire spiBusSCK, spiBusSCS;
wire spiBusSDI, spiBusSDO;

////////////////////////////
// -- Blocks/Instances -- //
////////////////////////////

// Synchronizer for JTAG inputs.
Synch2 JTAG_SYNCH[2:0] (
    .A(jtagSynchA),
    .Y(jtagSynchY),
    .clk(uproc_clk),
    .rstn(uproc_rstn)
);

// JTAG Port/Controller- Debug access port for development/programming.
JtagPort JTAG_PORT (
    // JTAG pin wires.
    .jtagTCK(jtagSynchTCK),
    .jtagTDI(jtagSynchTDI),
    .jtagTDO(jtagBaseTDO),
    .jtagTMS(jtagSynchTMS),
    
    // Status signals (from MCU, to user).
    .isBooted(/* TODO- populate */ 1'b1),
    .isPaused(/* TODO- populate */ 1'b1),
    
    // SRAM chip connector.
    .sramAddr(jtagSramAddr),
    .sramData(uproc_sramData),
    .sramWr(jtagSramWr),
    .sramEn(jtagSramEn),
    
    // Control signals (from user, to MCU).
    .enScanRelay(jtagEnScan),
    .enSPIRelay(jtagEnSPI),
    .enPaused(jtagDoPause),
    
    // Common signals.
    .clk(uproc_clk),
    .rstn(uproc_rstn)
);

// Memory Bus Controller- Determines who currently controls the memory bus.
MemoryController MEM_CTRL (
	// Processor control inputs.
	.procAddr(),
	.procWr(),
	.procEn(),
	
	// JTAG control inputs.
	.jtagAddr(jtagSramAddr),
	.jtagWr(jtagSramWr),
	.jtagEn(jtagSramEn),
	
	// Boot circuit inputs.
	.bootAddr(),
	.bootWr(),
	.bootEn(),
	
	// Control signals determining controlling circuits.
	.isPaused(/* TODO- populate */ 1'b1),
	.isBooted(/* TODO- populate */ 1'b1),
	
	// Selected control signals.
	.busAddr(memBusAddr),
	.busWr(memBusWr),
	.busRamEn(memBusRamEn),
	.busMapEn(memBusMapEn)
);

// External Storage Controller- determines who controls SPI access to storage.
StorageController SPI_CTRL (
    // Controls from memory-mapped peripheral.
    .mapEn(),
    
    // Controls from JTAG port.
    .jtagTCK(jtagSynchTCK),
    .jtagEn(jtagEnSPI),
    
    // Controls from Boot circuit.
    .bootEn(),
    
    // Control signals deciding which circuit is in control.
    .isPaused(/* TODO- populate */ 1'b1),
    .isBooted(/* TODO- populate */ 1'b1),
    
    // Selected controls sent over SPI connection.
    .spiSCK(spiBusSCK),
    .spiSCS(spiBusSCS),
    
    // Common signals.
    .clk(uproc_clk),
    .rstn(uproc_rstn)
);

//////////////////////////
// -- Connects/Logic -- //
//////////////////////////

// Synchronize JTAG pins for internal use.
assign jtagSynchA = {uproc_jtagTCK, uproc_jtagTDI, uproc_jtagTMS};
assign jtagSynchTCK = jtagSynchY[2];
assign jtagSynchTDI = jtagSynchY[1];
assign jtagSynchTMS = jtagSynchY[0];

// Connect memory bus controller to SRAM chip.
assign uproc_sramAddr = memBusAddr;
assign uproc_sramWr = memBusWr;
assign uproc_sramEn = memBusRamEn;

// Connect storage/SPI bus to SPI pins.
assign uproc_spiSCK = spiBusSCK;
assign uproc_spiSDO = jtagSynchTDI;
assign uproc_spiSCS = spiBusSCS;
Mux2 M0 (
	.A(uproc_spiSDI),
	.B(jtagBaseTDO),
	.S(jtagEnSPI),
	.Y(uproc_jtagTDO)
);

///////////////////////////////////////////////////////////
// -- TODO- test signals for development. TO DELETE!! -- //
///////////////////////////////////////////////////////////

assign test_word0 = {8'b0, 3'b0, jtagSynchTCK, 3'b0, spiBusSCK};
assign test_word1 = jtagSramAddr;

endmodule
