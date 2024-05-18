/*
 * MemoryController.v
 *
 * "'Mux' for controlling which circuit has control of the memory bus"
 */
module MemoryController (
    // Controls from processor core.
    input [15:0] i_coreAddr,
    input        i_coreWr,
    input        i_coreEn,
    
    // Controls from JTAG port.
    input [15:0] i_jtagAddr,
    input        i_jtagWr,
    input        i_jtagEn,
    
    // Controls from Boot circuit.
    input [15:0] i_bootAddr,
    input        i_bootWr,
    input        i_bootEn,
    
    // Control signals deciding which circuit is in control.
    input        i_isPaused,
    input        i_isBooted,

    // Control signal deciding when controller can drive memory bus.
    input        i_disableDrive,
    
    // Selected controls sent over memory bus.
    inout [15:0] io_memAddr,
    inout        io_memWr,
    inout        io_memExtEn,
    output       o_memMapEn
);

/*
 * Memory Controller- "Gated Mux" of Memory Bus Controls
 *
 * Routes memory control signals of responsible circuit to official memory bus
 * (based on MCU state machine signals). Also can stop driving memory bus to
 * prevent contention, as well as implements external vs mapped memory access.
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
 * 1) 'i_isPaused' should factor for MCU needing several cycles to pause
 * 2) Data line not included due to Verilog limits- data is strictly one net
 * 3) Assumes JTAG signals infer when bus can be driven while 'paused'
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// "Packed" control signals for easier selection.
wire [17:0] coreCtrl, jtagCtrl, bootCtrl;

// Nets related to mux-ing of control signals.
wire [17:0] busCtrl, hasBootedCtrl;

// Computed signals for enabling correct part of memory.
wire enExtAddr;

// Wires related to tristate driving memory control signals.
wire [17:0] triA, triY;
wire        triS;

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------
// Tristate controlling when memory controls can be applied/drive.
Tristate TRI[17:0] (
    .A(triA),
    .Y(triY),
    .S(triS)
);

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Pack input controls to ease readability.
assign coreCtrl = {i_coreEn, i_coreWr, i_coreAddr};
assign jtagCtrl = {i_jtagEn, i_jtagWr, i_jtagAddr};
assign bootCtrl = {i_bootEn, i_bootWr, i_bootAddr};

//------------------------------------------------------------------------------
// Determine which circuit should be in control.
Mux2 M0[17:0] ( // 'Running' vs. 'Paused' state of MCU
    .A(jtagCtrl),
    .B(coreCtrl),
    .S(i_isPaused),
    .Y(hasBootedCtrl)
);
Mux2 M1[17:0] ( // 'Booting' state vs. 'Running/Paused' states
    .A(hasBootedCtrl),
    .B(bootCtrl),
    .S(i_isBooted),
    .Y(busCtrl)
);

//------------------------------------------------------------------------------
// Compute enable signals for different addressable spaces.
assign enExtAddr = busCtrl[17] & ~(busCtrl[15] & busCtrl[14]);

//------------------------------------------------------------------------------
// Use tristate to only drive memory bus when allowed.
assign triA = {enExtAddr, busCtrl[16:0]};
assign triS = ~i_disableDrive;

//------------------------------------------------------------------------------
// Drive selected bus controls (as able).
assign io_memAddr  = triY[15:0];
assign io_memWr    = triY[16];
assign io_memExtEn = triY[17];
assign o_memMapEn  = busCtrl[17] & ~enExtAddr; // Mapped access- within MCU

endmodule
