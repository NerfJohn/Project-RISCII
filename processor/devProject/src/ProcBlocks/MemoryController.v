/*
 * MemoryController.v
 *
 * "'Mux' for controlling which circuit has control of the memory bus"
 */
module MemoryController (
    // Controls from processor.
    input [15:0] procAddr,
    input        procWr,
    input        procEn,
    
    // Controls from JTAG port.
    input [15:0] jtagAddr,
    input        jtagWr,
    input        jtagEn,
    
    // Controls from Boot circuit.
    input [15:0] bootAddr,
    input        bootWr,
    input        bootEn,
    
    // Control signals deciding which circuit is in control.
    input        isPaused,
    input        isBooted,
    
    // Selected controls sent over memory bus.
    output[15:0] busAddr,
    output       busWr,
    output       busRamEn,
     output       busMapEn
);

/*
 * Memory Controller:
 *
 * Large "mux" of various circuit controls for memory bus (where circuit in
 * control is effectively based on MCU's top-level state). Below describes
 * when each circuit has control and what addresses actually access.
 *
 * state   | control circuit | desc.
 * --------+-----------------+------
 * Booting | Boot Block      | MCU is copying binary into SRAM chip
 * Running | Processor Core  | MCU is running binary/program
 * Paused  | JTAG Port       | MCU paused for probing via JTAG
 *
 * section                 | address range   | access size
 * ------------------------+-----------------+------------
 * External SRAM chip      | 0x0000 - 0xBFFF | word (2-byte) access
 * Memory-mapped registers | 0xC000 - 0xFFFF | word (2-byte) access
 *
 * Design Notes:
 * 1) 'isPaused' should factor for MCU needing several cycles to pause
 * 2) Data line not included due to Verilog limits- data is strictly one net
 */

/////////////////////////
// -- Signals/Wires -- //
/////////////////////////

// "Packed" control signals for easier selection.
wire[17:0] procCtrl, jtagCtrl, bootCtrl;
wire[17:0] busCtrl;

// Intermediate signals between mux selection.
wire[17:0] hasBootedCtrls;

//////////////////////////
// -- Connects/Logic -- //
//////////////////////////

// Pack input controls to ease readability.
assign procCtrl = {procEn, procWr, procAddr};
assign jtagCtrl = {jtagEn, jtagWr, jtagAddr};
assign bootCtrl = {bootEn, bootWr, bootAddr};

// Determine which circuit should be in control.
Mux2 M0[17:0] ( // 'Running' vs. 'Paused' state of MCU
    .A(jtagCtrl),
    .B(procCtrl),
    .S(isPaused),
    .Y(hasBootedCtrls)
);
Mux2 M1[17:0] ( // 'Booting' state vs. 'Running/Paused' states
    .A(hasBootedCtrls),
    .B(bootCtrl),
    .S(isBooted),
    .Y(busCtrl)
);

// Unpack controls to begin controlling memory bus.
assign busAddr = busCtrl[15:0];
assign busWr   = busCtrl[16];

// Computer enable signals based on address being accessed.
assign busRamEn = ~(busCtrl[15] & busCtrl[14]) & busCtrl[17];
assign busMapEn = (busCtrl[15] & busCtrl[14]) & busCtrl[17];

endmodule
