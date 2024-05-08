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

/////////////////////////
// -- Signals/Wires -- //
/////////////////////////

// Synchronized JTAG wires.
wire[2:0] jtagSynchA, jtagSynchY;
wire      jtagSynchTCK, jtagSynchTDI, jtagSynchTMS;

// JTAG port/controller wires.
wire       jtagBaseTDO;
wire[15:0] jtagSramAddr, jtagSramData;
wire       jtagSramWr, jtagSramEn;
wire       jtagEnScan, jtagEnSPI;
wire       jtagDoPause;

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
    .sramData(jtagSramData),
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

//////////////////////////
// -- Connects/Logic -- //
//////////////////////////

// Synchronize JTAG pins for internal use.
assign jtagSynchA = {uproc_jtagTCK, uproc_jtagTDI, uproc_jtagTMS};
assign jtagSynchTCK = jtagSynchY[2];
assign jtagSynchTDI = jtagSynchY[1];
assign jtagSynchTMS = jtagSynchY[0];

///////////////////////////////////////////////////////////
// -- TODO- test signals for development. TO DELETE!! -- //
///////////////////////////////////////////////////////////

MemoryBus OO (
	.data(jtagSramData[0]),
	.en(~jtagSramWr),
	.sData(uproc_sramData[0]),
);

assign uproc_jtagTDO = jtagBaseTDO;

assign test_word0 = {3'b0, jtagSramEn, 3'b0, jtagDoPause, 3'b0, jtagEnScan, 3'b0, jtagEnSPI};
//assign test_word1 = jtagSramAddr;

endmodule
