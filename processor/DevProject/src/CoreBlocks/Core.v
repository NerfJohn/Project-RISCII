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
wire        memIBusy;
wire [15:0] memDAddr, memDData;
wire        memDWr,   memDSwp,  memDEn;
wire        memDRead, memDDone;
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
wire isPseInstr, isMemInstr;

// TODO- test signals, to delete after checking mem controller.
wire [15:0] dummyD, dummyQ;
wire        dummyEn;

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------
// Core's memory controller- handles accesses/schedule for instructions/data.
CoreMemController MEM_CTRL (
    // Instruction memory inputs (implied enable/read- also word addressable).
    .i_iAddr(memIAddr),
	 
	 // Instruction memory status signals.
	 .o_iIsBusy(memIBusy),
	 
	 // Data memory inputs.
	 .i_dAddr(memDAddr),
	 .i_dData(memDData),
	 .i_dWr(memDWr),
	 .i_dSwp(memDSwp),
	 .i_dEn(memDEn),
	 
	 // Data memory status signals.
	 .o_dIsReading(memDRead),
	 .o_dIsDone(memDDone),
    
    // Unified memory interface (data for writing, implied enable).
    .o_coreAddr(memCoreAddr),
    .io_coreData(io_memData), // direct connect- verilog inout limitations
    .o_coreWr(memCoreWr),
	 
	 // Common signals.
	 .i_clk(i_clk),
	 .i_rstn(i_rstn)
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
assign doStallPc    = i_startPause | isPseInstr | isMemInstr;
assign doClearPipe  = i_startPause | memIBusy;
assign doFreezePipe = isPseInstr | (isMemInstr & ~memDDone);

//------------------------------------------------------------------------------
// Compute control signals.
assign isPseInstr  = ~pipeInstr[15] & ~pipeInstr[14] &  // PSE = 0x3XXX
                      pipeInstr[13] &  pipeInstr[12];
assign isMemInstr  = ~pipeInstr[15] & ~pipeInstr[13] &  // LDR, STR, SWP
                     (pipeInstr[14] | pipeInstr[12]);

//------------------------------------------------------------------------------
// TODO- dummy test for core memory controller and pipe logic.

DffSynchEn DUMMY_REG[15:0] (
	.D(dummyD),
	.Q(dummyQ),
	.S(dummyEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

assign memDAddr = {12'b0, pipeInstr[3:1]};
assign memDData = dummyQ;
assign memDWr   = ~pipeInstr[12];
assign memDEn   = isMemInstr;
assign memDSwp  = ~pipeInstr[15] & ~pipeInstr[14] & ~pipeInstr[13] & pipeInstr[12];

assign dummyD   = io_memData;
assign dummyEn  = memDRead;

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
assign o_test_word1 = io_memData; //{3'b0, memIBusy, 3'b0, memDRead, 3'b0, memDDone, io_memData[3:0]};

endmodule
