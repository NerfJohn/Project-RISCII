/*
 * Core.v
 *
 * "Top level of core/software-executing logic of RISCII microprocessor"
 */
module Core (
    // Memory bus connection (for instructions/data- always enabled).
    output [15:0] o_memAddr,
    inout  [15:0] io_memData,
    output        o_memWr,
    
    // MCU state signals (pause source + multi-cycle pause).
    output        o_doPause,
    input         i_startPause,
    output        o_nowPaused,
    input         i_isBooted,
    
    // Common signals.
    input         i_clk,
    input         i_rstn,
    
   // TODO- Test signals for development; DELETE FOR PRODUCTION!!!
   output [15:0] o_test_word0,
   output [15:0] o_test_word1
);

/*
 * TODO- desc. module
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Computed signals for controlling pipeline flow of core.
wire doStallPc, doClearPipe, doFreezePipe;

// Wires related to core's memory controller.
wire [14:0] memIAddr;
wire [15:0] memCoreAddr;
wire        memCoreWr;

// Wires related to Program Counter (PC) register (word addressable).
wire [14:0] pcD, pcQ;

// Wires related to incrementor for PC addresses.
wire [14:0] pcIncA, pcIncY, pcIncCarry;

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------
// Core's memory controller- handles accesses/schedule for instructions/data.
CoreMemController MEM_CTRL (
    // Instruction memory inputs (implied enable/read- also word addressable).
    .i_iAddr(memIAddr),
    
    // Unified memory interface (data for writing, implied enable).
    .o_coreAddr(memCoreAddr),
    .io_coreData(io_memData), // direct connect- verilog inout limitations
    .o_coreWr(memCoreWr)
);

//------------------------------------------------------------------------------
// Program Counter (PC) Register- start of every instruction.
DffSynch PC[14:0] (
    .D(pcD),
    .Q(pcQ),
    .clk(i_clk),
    .rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Incrementor for PC- generates PC + 2 for jumps/next address.
Add1 PC_INC[14:0] ( // TODO- using as incrementor- refactor?
    .A(pcIncA),
    .B(15'b000000000000000),
    .I({pcIncCarry[13:0], 1'b1}), 
    .S(pcIncY),
    .O(pcIncCarry)
);

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute control signals to control pipeline.
assign doStallPc    = /* TODO- implement */ 1'b0;
assign doClearPipe  = /* TODO- implement */ 1'b0;
assign doFreezePipe = /* TODO- implement */ 1'b0;

//------------------------------------------------------------------------------
// Connect core parts to memory controller.
assign memIAddr = pcQ;

//------------------------------------------------------------------------------
// Determine next PC address to attempt to read.
assign pcD = pcIncY & {16{i_isBooted}}; // TODO- implement

//------------------------------------------------------------------------------
// Compute next PC address in sequence.
assign pcIncA = pcQ;

//------------------------------------------------------------------------------
// Connect memory controller to core's interface to the MCU.
assign o_memAddr = memCoreAddr;
assign o_memWr   = memCoreWr;

//------------------------------------------------------------------------------
// Determine pause controls based on core and pause details.
assign o_doPause   = 1'b0;
assign o_nowPaused = i_startPause;

//------------------------------------------------------------------------------
// TODO- test signals for development- TO DELETE!
assign o_test_word0 = 0;
assign o_test_word1 = 0;

endmodule
