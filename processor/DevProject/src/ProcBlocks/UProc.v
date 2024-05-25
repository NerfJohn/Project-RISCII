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
 * Boot Block           | State machine used to init MCU + memory devices
 *
 * == Notable Top-Level Nets ==
 * net name             | desc.
 * ---------------------+------
 * io_memData           | Bi-directional net for Memory Bus data signals
 * io_storeTDO          | "Input" net that, due to scanning, is bi-directional
 * boot_nowBooted       | Status/State signal for the MCU's overall state
 * 
 * Design Notes:
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Wires related to syncrhonizing reset pin input.
wire synch_rstnA, synch_rstnMid, synch_rstnY;

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

// Wires related to Boot Block.
wire [15:0] boot_memAddr;
wire        boot_memEn;
wire        boot_storeEn, boot_storeSDI;
wire        boot_nowBooted;

// Wires related to MCU output state/status signals.
wire bStatTriA,  pStatTriY;
wire bStatTriEn, pStatTriEn;

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------
// Special "synchronizer" for reset signal- avoids "who resets reset" question.
DffSynch RESET_SYNCH[1:0] (
	.D(2'b11),
	.Q({synch_rstnY, synch_rstnMid}),
	.clk(i_clk),
	.rstn({synch_rstnMid, synch_rstnA})
);

//------------------------------------------------------------------------------
// Synchronizer for JTAG input pins.
Synch2 JTAG_SYNCH[2:0] (
    .A(synch_jtagA),
    .Y(synch_jtagY),
    .clk(i_clk),
    .rstn(synch_rstnY)
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
	 .i_isBooted(boot_nowBooted),
	 .i_isPaused(/* TODO- implement w/ SM*/ jtag_doPause),
    
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
    .i_rstn(synch_rstnY)
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
	.i_rstn(synch_rstnY)
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
    
    // Controls from Boot circuit (only writes).
    .i_bootAddr(boot_memAddr),
    .i_bootEn(boot_memEn),
    
    // Control signals deciding which circuit is in control.
    .i_isPaused(/* TODO- implement w/ SM*/ jtag_doPause),
    .i_isBooted(boot_nowBooted),

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
    .i_bootEn(boot_storeEn),
    .i_bootSDI(boot_storeSDI),
    
    // Control signals deciding which circuit is in control.
    .i_isPaused(/* TODO- implement w/ SM*/ jtag_doPause),
    .i_isBooted(boot_nowBooted),
    
    // Selected controls sent over SPI connection.
    .io_storeSDI(io_storeSDI),
    .io_storeSCK(io_storeSCK),
    .io_storeSCS(io_storeSCS),
 
    // Control signal deciding when controller can drive memory bus.
    .i_disableDrive(jtag_scanEn), // JTAG can only assert in 'paused' state
    
    // Common signals.
    .i_clk(i_clk),
    .i_rstn(synch_rstnY)
);

//------------------------------------------------------------------------------
// Boot Block- first "controlling block", copies binary sections to memory.
BootBlock BOOT_BLOCK (
	// Memory bus connections (write only).
	.o_memAddr(boot_memAddr),
	.io_memData(io_memData),
	.o_memEn(boot_memEn),
	
	// Storage bus connections (core clock used for SCK).
	.o_storeEn(boot_storeEn),
	.o_storeSDI(boot_storeSDI),
	.i_storeSDO(io_storeSDO),
	
	// Generated control signal (origin of "booting" state).
	.o_nowBooted(boot_nowBooted),
	
	// Common signals.
	.i_clk(i_clk),
	.i_rstn(synch_rstnY)
);

//------------------------------------------------------------------------------
// Tristate to control report of status (boundary scanned pins).
Tristate BSTAT_TRI (
	.A(bStatTriA),
	.Y(io_isBooted),
	.S(bStatTriEn)
);
Tristate PSTAT_TRI (
	.A(pStatTriA),
	.Y(io_isPaused),
	.S(pStatTriEn)
);

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Synchronizer reset input pin for internal use.
assign synch_rstnA = i_rstn;

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

//------------------------------------------------------------------------------
// Handle driving status pins (vs. boundary scan register).
assign bStatTriA  = boot_nowBooted;
assign bStatTriEn = ~jtag_scanEn;   // JTAG can only assert in 'paused' state
assign pStatTriA  = /* TODO- implement */ jtag_doPause;
assign pStatTriEn = ~jtag_scanEn;   // JTAG can only assert in 'paused' state

///////////////////////////////////////////////////////////
// -- TODO- test signals for development. TO DELETE!! -- //
///////////////////////////////////////////////////////////

assign o_test_word0 = io_memAddr;
assign o_test_word1 = {3'b0, io_storeSCK, 3'b0, io_storeSCS, 3'b0, boot_nowBooted, 3'b0, jtag_doPause};

endmodule
