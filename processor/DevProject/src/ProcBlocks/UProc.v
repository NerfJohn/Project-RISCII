/*
 * UProc.v
 *
 * "Top level of Project RISCII's microprocessor design"
 */
module UProc (
    // Memory/SRAM chip connections (IO due to B-Scan).
    inout  [15:0] io_memAddr,
    inout  [15:0] io_memData,
    inout         io_memWr,
    inout         io_memEn,

    // Storage/EEPROM SPI connections (IO due to B-Scan).
    inout         io_storeSCK,
    inout         io_storeSDI,
    inout         io_storeSDO,
    inout         io_storeSCS,

    // GPIO connections.
    inout  [15:0] io_gpioPin,

    // State/Status connections (IO due to B-Scan).
    inout         io_isBooted,
    inout         io_isPaused,
    
    // JTAG port connections.
    input         i_jtagTCK,
    input         i_jtagTDI,
    output        o_jtagTDO,
    input         i_jtagTMS,

    // Common signals.
    input         i_clk,
    input         i_rstn,

    // TODO- Test signals for development; DELETE FOR PRODUCTION!!!
    output [15:0] o_test_word0,
    output [15:0] o_test_word1
);

/*
 * Microprocessor ("UProc")- Top-level Design of the MCU
 *
 * This file combines all pieces of the MCU into one, working unit. Beyond this
 * file lies a chasis to tie the MCU to the physical FPGA and/or the "open road"
 * (ie physical hardware making up the board).
 *
 * TODO- summary of module + design notes. Keep list of modules- order later.
 *
 * == Top-Level Modules/Blocks ==
 * module name          | desc.
 * ---------------------+------
 * JTAG Synch           | Synchronizer for JTAG pins
 * JTAG Port            | Port/Controller for handling JTAG signals
 *
 *
 * == Notable Top-Level Nets ==
 * net name             | desc.
 * ---------------------+------
 * io_memData           | Bi-directional net for Memory Bus data signals
 * 
 * Design Notes:
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Wires related to JTAG pin's synchronizer.
wire [2:0] synch_jtagA, synch_jtagY;
wire       jtag_TCK, jtag_TDI, jtag_TDO, jtag_TMS;

// Wires related to JTAG port controls.
wire [15:0] jtag_memAddr;
wire        jtag_memWr, jtag_memEn;
wire        jtag_storeEn;
wire        jtag_scanEn, jtag_scanShift;
wire        jtag_doPause;

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------
// Synchronizer for JTAG input pins.
Synch2 JTAG_SYNCH[2:0] (
    .A(synch_jtagA),
    .Y(synch_jtagY),
    .clk(i_clk),
    .rstn(i_rstn)
);

//------------------------------------------------------------------------------
// JTAG Port/Controller- port to allow for debugging/programming.
JtagPort JTAG_PORT (
    // Typical I/O of JTAG port.
    .i_TCK(jtag_TCK),
    .i_TDI(jtag_TDI),
    .o_TDO(jtag_TDO),
    .i_TMS(jtag_TMS),
    
	 // MCU state for status report + control signal computation.
	 .i_isBooted(/* TODO- dummy */ 1'b1),
	 .i_isPaused(/* TODO- dummy */ jtag_doPause),
    
	 // Memory control connections.
	 .o_memAddr(jtag_memAddr),
	 .io_memData(io_memData),
	 .o_memWr(jtag_memWr),
	 .o_memEn(jtag_memEn),
	 
	 // Storage control connections.
	 .o_storeEn(jtag_storeEn),
	 
	 // B-Scan control connections.
	 .o_scanEn(jtag_scanEn),
    .o_scanShift(jtag_scanShift),
    
	 // Run/Pause control signal.
	 .o_doPause(jtag_doPause),
    
    // Common signals.
    .i_clk(i_clk),
    .i_rstn(i_rstn)
);

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Synchronize JTAG pins for internal use.
assign synch_jtagA = {i_jtagTCK, i_jtagTDI, i_jtagTMS};
assign jtag_TCK    = synch_jtagY[2];
assign jtag_TDI    = synch_jtagY[1];
assign jtag_TMS    = synch_jtagY[0];

///////////////////////////////////////////////////////////
// -- TODO- test signals for development. TO DELETE!! -- //
///////////////////////////////////////////////////////////

assign o_test_word0 = io_memData;
assign o_test_word1 = jtag_memAddr;

endmodule
