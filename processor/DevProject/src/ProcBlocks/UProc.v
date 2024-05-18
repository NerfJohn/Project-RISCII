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
 * B-Scan Register      | Boundary Scan Register for HW Debugging
 * Memory Controller    | Control logic to handle who's in control of memory
 * Storage Controller   | Control logic to handle who's in control of storage
 *
 * == Notable Top-Level Nets ==
 * net name             | desc.
 * ---------------------+------
 * io_memData           | Bi-directional net for Memory Bus data signals
 * io_storeTDO          | "Input" net that, due to scanning, is bi-directional
 * 
 * Design Notes:
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Wires related to JTAG pin's synchronizer.
wire [2:0] synch_jtagA, synch_jtagY;
wire       jtag_TCK, jtag_TDI, jtag_TMS;

// Wires related to JTAG port controls.
wire [15:0] jtag_memAddr;
wire        jtag_memWr, jtag_memEn;
wire        jtag_storeEn;
wire        jtag_scanEn, jtag_scanShift;
wire        jtag_doPause;
wire        jtag_TDO;

// Wires related to boundary scan register.
wire bscan_shiftOut;

// Nets related to muxing of JTAG's TDO pin.
wire jtagTDO_maybeStore;

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
	 .i_isPaused(/* TODO- dummy */ 1'b1),
    
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

//------------------------------------------------------------------------------
// Boundary Scan Register- probes in/outgoing pins for HW debugging purposes.
BScanRegister BSCAN_REG (
	// Typical shift register IO.
	.i_shiftIn(jtag_TDI),
	.o_shiftOut(bscan_shiftOut),
	
	// Memory connector probe lines.
	.io_memAddr(io_memAddr),
	.io_memData(io_memData),
	.io_memWr(io_memWr),
	.io_memEn(io_memEn),
	
	// Storage connector probe lines.
	.io_storeSCK(io_storeSCK),
	.io_storeSDI(io_storeSDI),
	.io_storeSDO(io_storeSDO),
	.io_storeSCS(io_storeSCS),
	
	// GPIO connector probe lines.
	.io_gpioPin(io_gpioPin),
	
	// Status connection probe lines.
	.io_isBooted(io_isBooted),
	.io_isPaused(io_isPaused),
	
	// Control lines to enable settings/shifting.
	.i_canDrive(jtag_scanEn),
	.i_doShift(jtag_scanShift),
	
	// Common signals.
	.i_clk(jtag_TCK),
	.i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Memory Controller- selects circuit to control memory bus pins.
MemoryController MEM_CTRL (
    // Controls from processor core.
    .i_coreAddr(),
    .i_coreWr(),
    .i_coreEn(),
    
    // Controls from JTAG port.
    .i_jtagAddr(jtag_memAddr),
    .i_jtagWr(jtag_memWr),
    .i_jtagEn(jtag_memEn),
    
    // Controls from Boot circuit.
    .i_bootAddr(),
    .i_bootWr(),
    .i_bootEn(),
    
    // Control signals deciding which circuit is in control.
    .i_isPaused(/* TODO- dummy */ 1'b1),
    .i_isBooted(/* TODO- dummy */ 1'b1),

    // Control signal deciding when controller can drive memory bus.
    .i_disableDrive(jtag_scanEn), // JTAG can only assert in 'paused' state
    
    // Selected controls sent over memory bus.
    .io_memAddr(io_memAddr),
    .io_memWr(io_memWr),
    .io_memExtEn(io_memEn),
    .o_memMapEn()
);

//------------------------------------------------------------------------------
// Storage Controller- selects circuit to control storage bus pins.
StorageController STORE_CTRL (
    // Controls from memory-mapped peripheral.
    .i_mapEn(),
    .i_mapSDI(),
    
    // Controls from JTAG port.
    .i_jtagTCK(jtag_TCK),
    .i_jtagEn(jtag_storeEn),
    .i_jtagSDI(jtag_TDI),
    
    // Controls from Boot circuit.
    .i_bootEn(),
    .i_bootSDI(),
    
    // Control signals deciding which circuit is in control.
    .i_isPaused(/* TODO- dummy */ 1'b1),
    .i_isBooted(/* TODO- dummy */ 1'b1),
    
    // Selected controls sent over SPI connection.
    .io_storeSDI(io_storeSDI),
    .io_storeSCK(io_storeSCK),
    .io_storeSCS(io_storeSCS),
 
    // Control signal deciding when controller can drive memory bus.
    .i_disableDrive(jtag_scanEn), // JTAG can only assert in 'paused' state
    
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

//------------------------------------------------------------------------------
// Determine which circuit should report back over JTAG port.
Mux2 M0 (
	.A(io_storeSDO),
	.B(jtag_TDO),
	.S(jtag_storeEn),
	.Y(jtagTDO_maybeStore)
);
Mux2 M1 (
	.A(bscan_shiftOut),
	.B(jtagTDO_maybeStore),
	.S(jtag_scanShift),
	.Y(o_jtagTDO)
);

///////////////////////////////////////////////////////////
// -- TODO- test signals for development. TO DELETE!! -- //
///////////////////////////////////////////////////////////

assign o_test_word0 = io_memData;
assign o_test_word1 = {3'b0, io_storeSCK, 3'b0, io_storeSCS, 3'b0, io_storeSDI, 3'b0, io_storeSDO};

endmodule
