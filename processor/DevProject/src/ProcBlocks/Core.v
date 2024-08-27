/*
 * Core.v
 *
 * "Core processing unit of the uP- executioner of program code and data"
 */
module Core (
	// Memory port connections.
	input  [15:0] i_memDataIn,
	output [15:0] o_memAddr,
	output [15:0] o_memDataOut,
	output        o_memWr,
	
	// uP State connections.
	input         i_smIsBooted,
	input         i_smStartPause,
	output        o_smNowPaused,
	
	// Reported control connections.
	output [15:0] o_reportSP,
	output [14:0] o_reportPC,
	output        o_reportHLT,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * TODO- desc.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Computed controls (based on core state/hazards).
wire        doStallPC, doFreezeEXE, doClearEXE;

// Pause wires.
wire        pauseD, pauseQ;

// Program Counter wires.
wire [14:0] pcAddA, pcAddB, pcAddS;
wire        pcAddI;
wire [14:0] pcD, pcQ;
wire        pcEn;

// Execute register wires.
wire [15:0] exeD, exeQ;
wire        exeEn;

// Control logic wires.
wire [3:0]  ctrlOpcode;
wire        ctrlWrReg;
wire        ctrlIsHLT;

// Reg file wires.
wire [2:0]  regRAddr1;
wire [15:0] regRData1;
wire [15:0] regWData;
wire [2:0]  regWAddr;
wire        regWEn;
wire [15:0] regReportSP;

// Immediate block wires.
wire [7:0]  immInstrImm;
wire [15:0] immGenImm;

// ALU wires.
wire [15:0] aluSrcA, aluSrcB;
wire        aluOpSel;
wire [15:0] aluResult;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Pause Output- local PAUSED state of core (and confirmation of pause state).
DffSynch LOCAL_PAUSE (
	.D(pauseD),
	.Q(pauseQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Program Counter (PC)- instruction address to fetch (and increment afterward).
Add15 PC_INC (
	.A(pcAddA),
	.B(pcAddB),
	.I(pcAddI),
	.S(pcAddS)
);
DffSynchEn PC[14:0] (
	.D(pcD),
	.Q(pcQ),
	.S(pcEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Execute Register- holds instruction being executed.
DffSynchEn EXECUTE[15:0] (
	.D(exeD),
	.Q(exeQ),
	.S(exeEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Control Logic- determines actions needed to execute current instruction.
CtrlLogic CTRL_LOGIC (
	// Input opcode connection.
	.i_opcode(ctrlOpcode),
	
	// Control outputs.
	.o_wrReg(ctrlWrReg),
	.o_isHLT(ctrlIsHLT)
);

//------------------------------------------------------------------------------
// Register File- registers directly accessible to the instructions.
RegFile REG_FILE (
	// Read connections.
	.i_rAddr1(regRAddr1),
	.o_rData1(regRData1),
	
	// Write connections.
	.i_wData(regWData),
	.i_wAddr(regWAddr),
	.i_wEn(regWEn),
	
	// Stack pointer connection.
	.o_reportSP(regReportSP),
	
	// Common signals.
	.i_clk(i_clk),
	.i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Immediate Block- extracts/generates 16-bit immediate from instruction.
ImmBlock IMM_BLOCK (
	// Input instruction connection.
	.i_instrImm(immInstrImm),
	
	// Output immediate connection.
	.o_genImm(immGenImm)
);

//------------------------------------------------------------------------------
// Arithmetic Logic Unit (ALU)- heart of processor's computational abilities.
Alu ALU (
	// Operand connections.
	.i_srcA(aluSrcA),
	.i_srcB(aluSrcB),
	
	// Opcode connections.
	.i_opSel(aluOpSel),
	
	// Results connections.
	.o_result(aluResult)
);
	
////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute core/hazard based controls.
assign doStallPC   = ~i_smIsBooted                  // preserve 0x0000
                     | i_smStartPause               // preserve next instr
						   | pauseQ                       // officially "paused"
							| ctrlIsHLT;                   // stop core flow
assign doFreezeEXE = (ctrlIsHLT & ~i_smStartPause); // keep HLT till pause
assign doClearEXE  = ~i_smIsBooted                  // don't run in booting
                     | pauseQ                       // don't run in paused
							| i_smStartPause;              // prep to pause

//------------------------------------------------------------------------------
// Handle pause inputs.
assign pauseD = i_smStartPause;

//------------------------------------------------------------------------------
// Handle PC inputs.
assign pcAddA = pcQ;
assign pcAddB = 15'b000000000000000; // using as incrementor
assign pcAddI = 1'b1;                // using as incrementor
assign pcD    = pcAddS;
assign pcEn   = ~doStallPC;

//------------------------------------------------------------------------------
// Handle execute inputs.
assign exeD  = i_memDataIn & {16{~doClearEXE}}; // "clear"  > "read"
assign exeEn = ~doFreezeEXE;                    // "freeze" > "clear"

//------------------------------------------------------------------------------
// Handle Control logic inputs.
assign ctrlOpcode = exeQ[15:12];

//------------------------------------------------------------------------------
// Handle Register File inputs.
assign regRAddr1 = exeQ[11:9];
assign regWData  = aluResult;
assign regWAddr  = exeQ[11:9];
assign regWEn    = ctrlWrReg;

//------------------------------------------------------------------------------
// Handle Immediate Block inputs.
assign immInstrImm = exeQ[7:0];

//------------------------------------------------------------------------------
// Handle ALU inputs.
assign aluSrcA  = regRData1;
assign aluSrcB  = immGenImm;
assign aluOpSel = exeQ[8];

//------------------------------------------------------------------------------
// TODO- implement.
assign o_memAddr    = {1'b0, pcQ};
assign o_memDataOut = 16'hfeed;
assign o_memWr      = 1'b0;

//------------------------------------------------------------------------------
// Drive state machine outputs.
assign o_smNowPaused = pauseQ;

//------------------------------------------------------------------------------
// TODO- implement.
assign o_reportPC  = pcQ;
assign o_reportSP  = regReportSP;
assign o_reportHLT = ctrlIsHLT;

endmodule
