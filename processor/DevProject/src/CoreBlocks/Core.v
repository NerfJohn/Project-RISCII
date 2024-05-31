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

// Wires related to core's memory controller.
wire [14:0] memIAddr;
wire [15:0] memCoreAddr;
wire        memCoreWr;

// Wires related to Program Counter (PC) register (word addressable).
wire [14:0] pcD, pcQ;
wire        pcRstn;

// Wires related to incrementor for PC addresses.
wire [14:0] pcIncA, pcIncY, pcIncCarry;

// Wires related to register implementing "pipline" of core.
wire [31:0] pipeD, pipeQ;
wire        pipeEn;
wire        pipeClear;
wire [14:0] pipePc2;
wire [15:0] pipeInstr;

// Computed signals for Hazard detection/resolution.
wire doStallPc, doClearPipe, doFreezePipe;

// Computed signals for controlling core's EXE resources.
wire isPseInstr;

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
    .rstn(pcRstn)
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

//------------------------------------------------------------------------------
// Pipeline register- split's core into FETCH and EXECUTE/MEMORY stages.
DffSynchEn PIPE_REG[31:0] (
	.D(pipeD),
	.Q(pipeQ),
	.S(pipeEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Connect core parts to memory controller.
assign memIAddr = pcQ;

//------------------------------------------------------------------------------
// Determine next PC address to attempt to read.
Mux2 M0[14:0] (
	.A(pcQ),
	.B(pcIncY),
	.S(doStallPc),
	.Y(pcD)
);
assign pcRstn = i_rstn & i_isBooted; // wait at 0x0000 while booting

//------------------------------------------------------------------------------
// Compute next PC address in sequence.
assign pcIncA = pcQ;

//------------------------------------------------------------------------------
// Handle pipeline register logic- transitioning data between stages.
assign pipeD     = {doClearPipe, pcIncY, io_memData};
assign pipeEn    = ~doFreezePipe & i_isBooted;      // spin (resets to NOP)
assign pipeClear = pipeQ[31];
assign pipePc2   = pipeQ[30:16];
assign pipeInstr = {pipeQ[15:12] & {4{~pipeClear}}, // force NOP (0x0XXX)
                    pipeQ[11:0]
						 };

//------------------------------------------------------------------------------
// Compute hazard signals.
assign doStallPc    = i_startPause | isPseInstr;
assign doClearPipe  = i_startPause;
assign doFreezePipe = isPseInstr;

//------------------------------------------------------------------------------
// Compute control signals.
assign isPseInstr = ~pipeInstr[15] & ~pipeInstr[14] & // ie PSE = 0x3XXX
                    pipeInstr[13] & pipeInstr[12];

//------------------------------------------------------------------------------
// Connect memory controller to core's interface to the MCU.
assign o_memAddr = memCoreAddr;
assign o_memWr   = memCoreWr;

//------------------------------------------------------------------------------
// Determine pause controls based on core and pause details.
assign o_doPause   = isPseInstr;
assign o_nowPaused = i_startPause & (pipeClear | isPseInstr); // clear or PSE

//------------------------------------------------------------------------------
// TODO- test signals for development- TO DELETE!
assign o_test_word0 = pcQ;
assign o_test_word1 = io_memData; //pipeInstr;

endmodule
