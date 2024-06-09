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
	 
	 // Values used by memory mapped peripherals for computations.
	 output [14:0] o_pcValue,
    
    // Common signals.
    input         i_clk,
    input         i_rstn
);

/*
 * TODO- desc. module
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Wires related to the core's memory controller.
wire [14:0] memIAddr;
wire        memIBusy;
wire [14:0] memDAddr;
wire [15:0] memDData;
wire        memDWr,   memDSwp,  memDEn;
wire        memDRead, memDDone;
wire [15:0] memCoreAddr;
wire        memCoreWr;

// Wires related to maintaining "flow" of core execution.
wire        doStallPc, doClearPipe, doFreezePipe;

// Wires related to PC counter + incrementor.
wire [14:0] pcD,    pcQ;
wire        pcRstn;
wire [14:0] pcIncA, pcIncY, pcIncCarry;

// Wires related to the pipeline registers between stages.
wire [15:0] pipeInstrD, pipeInstrQ;

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
// Program Counter (PC) Register + incrementor- start of any instruction.
DffSynch PC[14:0] (
    .D(pcD),
    .Q(pcQ),
    .clk(i_clk),
    .rstn(i_rstn)
);
Add1 PC_INC[14:0] (              // TODO- using as inc- refactor?
	.A(pcIncA),
	.B(15'b000000000000000),
	.I({pcIncCarry[13:0], 1'b1}),
	.S(pcIncY),
	.O(pcIncCarry)
);

//------------------------------------------------------------------------------
// Set of registers dividing core into a two stage pipeline.
DffSynch PIPE_INSTR[15:0] (
	.D(pipeInstrD),
	.Q(pipeInstrQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Connect memory controller to instruction/data sources.
assign memIAddr = pcQ;
assign memDAddr = 15'b0;
assign memDData = 16'b0;
assign memDWr   = 1'b0;
assign memDSwp  = 1'b0;
assign memDEn   = 1'b0;

//------------------------------------------------------------------------------
// Handle controls maintaining flow of execution in core processor.
assign doStallPc    = i_startPause;
assign doClearPipe  = i_startPause;
assign doFreezePipe = 1'b0;

//------------------------------------------------------------------------------
// Handle PC incrementing + selection.
assign pcIncA = pcQ;
assign pcD    = pcIncY;
assign pcRstn = i_rstn & i_isBooted; // "boot" to 0x0

//------------------------------------------------------------------------------
// Save results of FETCH stage for execution.
assign pipeInstrD = io_memData;

//------------------------------------------------------------------------------
// Connect memory controller to core's interface to the MCU.
assign o_memAddr = memCoreAddr;
assign o_memWr   = memCoreWr;

//------------------------------------------------------------------------------
// Determine pause controls based on core and pause details.
assign o_doPause   = 0;
assign o_nowPaused = i_startPause;

// TODO- implement later. For now, convenient port for debug.
assign o_pcValue = pipeInstrQ[15:1];

endmodule
