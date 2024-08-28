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
wire [14:0] exePCD, exePCQ;

// Control logic wires.
wire [3:0]  ctrlOpcode;
wire        ctrlAllowJmp;
wire        ctrlWrReg, ctrlWrCC;
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
wire [3:0]  immInstrOpcode;
wire [15:0] immGenImm;

// ALU wires.
wire [15:0] aluSrcA, aluSrcB;
wire        aluOpSel;
wire [15:0] aluResult;
wire [3:0]  aluCcodes;

// Condition Code wires.
wire [3:0]  ccD, ccQ;
wire        ccEn;

// Jump PC wires.
wire [14:0] jmpAddA, jmpAddB, jmpAddS;
wire        jmpAddI;
wire [14:0] jmpAddr;
wire        jmpCodeMatch, jmpDoJmp;

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
// Execute Register- holds instruction (and PC+2) being executed.
DffSynchEn EXECUTE[15:0] (
	.D(exeD),
	.Q(exeQ),
	.S(exeEn),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynch EXE_PC[14:0] (
	.D(exePCD),
	.Q(exePCQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Control Logic- determines actions needed to execute current instruction.
CtrlLogic CTRL_LOGIC (
	// Input opcode connection.
	.i_opcode(ctrlOpcode),
	
	// Control outputs.
	.o_allowJmp(ctrlAllowJmp),
	.o_wrReg(ctrlWrReg),
	.o_wrCC(ctrlWrCC),
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
	.i_instrOpcode(immInstrOpcode),
	
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
	.o_result(aluResult),
	.o_ccodes(aluCcodes)
);

//------------------------------------------------------------------------------
// Condition Codes- results of previous data operations.
DffSynchEn CC[3:0] (
	.D(ccD),
	.Q(ccQ),
	.S(ccEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Jump PC Adder- calculate "jump" address/computations.
Add15 JMP_ADDER (
	.A(jmpAddA),
	.B(jmpAddB),
	.I(jmpAddI),
	.S(jmpAddS)
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
                     | jmpDoJmp                     // jump cancels EXE
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
Mux2 M0[14:0] (
	.A(jmpAddr),                      // Jumping? Set PC with new address
	.B(pcAddS),                       // No?      Just increment
	.S(jmpDoJmp),
	.Y(pcD)
);
assign pcEn   = ~doStallPC | jmpDoJmp;

//------------------------------------------------------------------------------
// Handle execute inputs.
assign exeD   = i_memDataIn & {16{~doClearEXE}}; // "clear"  > "read"
assign exeEn  = ~doFreezeEXE;                    // "freeze" > "clear"
assign exePCD = pcAddS;

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
assign immInstrImm    = exeQ[7:0];
assign immInstrOpcode = exeQ[15:12];

//------------------------------------------------------------------------------
// Handle ALU inputs.
assign aluSrcA  = regRData1;
assign aluSrcB  = immGenImm;
assign aluOpSel = exeQ[8];

//------------------------------------------------------------------------------
// Handle CC inputs.
assign ccD  = aluCcodes;
assign ccEn = ctrlWrCC;

//------------------------------------------------------------------------------
// Handle jump adder inputs/jump computations.
assign jmpAddA      = exePCQ;
assign jmpAddB      = immGenImm[15:1];
assign jmpAddI      = 1'b0;
assign jmpAddr      = jmpAddS;
assign jmpCodeMatch = (|(exeQ[11:9] & ccQ[3:1])) & (~exeQ[8] | ccQ[0]);
assign jmpDoJmp     = ctrlAllowJmp & jmpCodeMatch;

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
