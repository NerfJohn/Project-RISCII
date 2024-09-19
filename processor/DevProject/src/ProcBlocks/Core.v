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
	
	// Interrupt connections.
	input  [3:0]  i_intCode,
	input         i_intEn,
	
	// Reported control connections.
	output [15:0] o_reportSP,
	output [14:0] o_reportPC,
	output        o_reportHLT,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * Main processing unit of the uP- executing instructions within 2(.5) stages.
 *
 * Implements FETCH-EXECUTE stages with pause and interrupt signals as "side
 * inputs" and SW debug/halt details as "side outputs". A memory interface is
 * used to handle memory operations on behalf of the EXECUTE stage.
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
wire [14:0] pcIntAddr, pcCoreAddr;

// Execute register wires.
wire [15:0] exeD, exeQ;
wire        exeEn;
wire [14:0] exePCD, exePCQ;

// Control logic wires.
wire [3:0]  ctrlOpcode;
wire [2:0]  ctrlAluOp;
wire        ctrlAltSrc1, ctrlAltSrc2, ctrlAltSel;
wire        ctrlUseImm, ctrlAllowImm;
wire        ctrlUseJmp, ctrlAllowJmp;
wire        ctrlWrReg, ctrlWrCC, ctrlWrMem;
wire        ctrlIsMemInstr;
wire        ctrlIsSWP, ctrlIsJPR, ctrlIsHLT;

// Reg file wires.
wire [2:0]  regRAddr1, regRAddr2;
wire [15:0] regRData1, regRData2;
wire [15:0] regWData;
wire [2:0]  regWAddr;
wire        regWEn;
wire [15:0] regReportSP;
wire [15:0] regLocalSrc;

// Immediate block wires.
wire [7:0]  immInstrImm;
wire [3:0]  immInstrOpcode;
wire [15:0] immGenImm;

// ALU wires.
wire [15:0] aluSrcA, aluSrcB;
wire [2:0]  aluOpCode;
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
wire        jmpCodeMatch, jmpDoJmp, jmpDoRti;

// Mem controller wires.
wire [14:0] memIAddr, memDAddr;
wire [15:0] memDData;
wire        memDEn, memDWr, memDSwp;
wire        memDRead, memDEnd;
wire [15:0] memCoreMemAddr, memCoreMemData;
wire        memCoreMemWr;

// Interrupt wires.
wire        intDoInt;
wire        intD, intQ, intEn;

// Shadow register wires.
wire [14:0] shadowPCD, shadowPCQ;
wire        shadowPCEn;
wire [3:0]  shadowCCD, shadowCCQ;
wire        shadowCCEn;

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
	.o_aluOp(ctrlAluOp),
	.o_altSrc1(ctrlAltSrc1),
	.o_altSrc2(ctrlAltSrc2),
	.o_altSel(ctrlAltSel),
	.o_useImm(ctrlUseImm),
	.o_allowImm(ctrlAllowImm),
	.o_useJmp(ctrlUseJmp),
	.o_allowJmp(ctrlAllowJmp),
	.o_wrReg(ctrlWrReg),
	.o_wrCC(ctrlWrCC),
	.o_wrMem(ctrlWrMem),
	.o_isMemInstr(ctrlIsMemInstr),
	.o_isSWP(ctrlIsSWP),
	.o_isJPR(ctrlIsJPR),
	.o_isHLT(ctrlIsHLT)
);

//------------------------------------------------------------------------------
// Register File- registers directly accessible to the instructions.
RegFile REG_FILE (
	// Read connections.
	.i_rAddr1(regRAddr1),
	.i_rAddr2(regRAddr2),
	.o_rData1(regRData1),
	.o_rData2(regRData2),
	
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
	.i_opCode(aluOpCode),
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

//------------------------------------------------------------------------------
// Core Memory Controller- handles read/writes between instructions and data.
CoreMem CORE_MEM (
	// Access connections.
	.i_iAddr(memIAddr),
	.i_dAddr(memDAddr),
	.i_dData(memDData),
	
	// "Control" inputs (generally data driven).
	.i_dEn(memDEn),
   .i_dWr(memDWr),
	.i_dSwp(memDSwp),
	
	// Status outputs (to core).
	.o_dRead(memDRead),
	.o_dEnd(memDEnd),
	
	// Controller outputs (to rest of uP).
	.o_coreMemAddr(memCoreMemAddr),
	.o_coreMemData(memCoreMemData),
	.o_coreMemWr(memCoreMemWr),
	
	// Common signals.
	.i_clk(i_clk),
	.i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Interrupt Flop- tracks if core is running app code or interrupt.
DffSynchEn INT_MODE (
	.D(intD),
	.Q(intQ),
	.S(intEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Shadow registers- used to store context info during interrupt.
DffSynchEn SHADOW_PC[14:0] (
	.D(shadowPCD),
	.Q(shadowPCQ),
	.S(shadowPCEn),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn SHADOW_CC[3:0] (
	.D(shadowCCD),
	.Q(shadowCCQ),
	.S(shadowCCEn),
	.clk(i_clk),
	.rstn(i_rstn)
);
	
////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute core/hazard based controls.
assign doStallPC   = ~i_smIsBooted                  // preserve 0x0000
                     | ctrlIsMemInstr               // serving MEM cycles
                     | i_smStartPause               // preserve next instr
						   | pauseQ                       // officially "paused"
							| ctrlIsHLT;                   // stop core flow
assign doFreezeEXE = (ctrlIsHLT & ~i_smStartPause)  // keep HLT till pause
                     |(ctrlIsMemInstr & ~memDEnd);  // keep MEM till finishing
assign doClearEXE  = ~i_smIsBooted                  // don't run in booting
                     | memDEnd                      // "evict" MEM op
                     | jmpDoJmp                     // jump cancels EXE
							| intDoInt                     // implicit "jump"
							| jmpDoRti                     // special "jump"
                     | pauseQ                       // don't run in paused
							| i_smStartPause;              // prep to pause

//------------------------------------------------------------------------------
// Handle pause inputs.
assign pauseD = i_smStartPause; // TODO- wait for mems?

//------------------------------------------------------------------------------
// Handle PC inputs.
assign pcAddA = pcQ;
assign pcAddB = 15'b000000000000000;     // using as incrementor
assign pcAddI = 1'b1;                    // using as incrementor
Mux2 M0[14:0] (
	.A(jmpAddr),                          // Jumping? Use jump address
	.B(pcAddS),                           // No?      Use incremented address
	.S(jmpDoJmp),
	.Y(pcCoreAddr)
);
Mux2 M1[14:0] (
	.A({9'b000000000, i_intCode, 2'b00}), // INT? Set to vector address
	.B(shadowPCQ),                        // RTI? Set to RTI address
	.S(intDoInt),
	.Y(pcIntAddr)
);
Mux2 M2[14:0] (
	.A(pcIntAddr),                        // INT signals? Process INT addrs
	.B(pcCoreAddr),                       // No?          Process "core" addrs
	.S(intDoInt | jmpDoRti),
	.Y(pcD)
);
assign pcEn   = ~doStallPC | jmpDoJmp | intDoInt | jmpDoRti;

//------------------------------------------------------------------------------
// Handle execute inputs.
assign exeD   = i_memDataIn & {16{~doClearEXE}}; // "clear"  > "read"
assign exeEn  = ~doFreezeEXE;                    // "freeze" > "clear"
assign exePCD = pcAddS;

//------------------------------------------------------------------------------
// Handle Control logic inputs.
assign ctrlOpcode   = exeQ[15:12];

//------------------------------------------------------------------------------
// Handle Register File inputs.
Mux2 M3[2:0] (
	.A(exeQ[11:9]),             // Alt source? read LBI's DST
	.B(exeQ[8:6]),              // No?         read typical SR1
	.S(ctrlAltSrc1),
	.Y(regRAddr1)
);
Mux2 M4[2:0] (
	.A(exeQ[11:9]),             // Alt source? read STR/SWP SRC
	.B(exeQ[2:0]),              // No?         read typical SR2
	.S(ctrlAltSrc2),
	.Y(regRAddr2)
);
Mux2 M5[15:0] (
	.A({exePCQ, 1'b0}),         // Jumping? source from PC+2
	.B(aluResult),              // No?      source from ALU
	.S(jmpDoJmp),
	.Y(regLocalSrc)
);
Mux2 M6[15:0] (
	.A(i_memDataIn),            // MEM read?  source from MEM
	.B(regLocalSrc),            // No?        process local sources
	.S(memDRead),
	.Y(regWData)
);
assign regWAddr  = exeQ[11:9];
assign regWEn    = ctrlWrReg | memDRead; // MEM handles own permission

//------------------------------------------------------------------------------
// Handle Immediate Block inputs.
assign immInstrImm    = exeQ[7:0];
assign immInstrOpcode = exeQ[15:12];

//------------------------------------------------------------------------------
// Handle ALU inputs.
assign aluSrcA   = regRData1;
Mux2 M7[15:0] (
	.A(immGenImm),                             // Use Imm? Imm is Src2
	.B(regRData2),                             // No?      Read 2 is Src2
	.S(ctrlUseImm | (ctrlAllowImm & exeQ[5])),
	.Y(aluSrcB)
);
assign aluOpCode = ctrlAluOp;
Mux2 M8 (
	.A(exeQ[8]),                               // Alt select? Use LBI flag
	.B(exeQ[4]),                               // No?         Use SHR flag
	.S(ctrlAltSel),
	.Y(aluOpSel)
);

//------------------------------------------------------------------------------
// Handle CC inputs.
Mux2 M9[3:0] (
	.A(shadowCCQ),       // Returning from interrupt? Use saved codes
	.B(aluCcodes),       // No?                       Use typical codes
	.S(jmpDoRti),
	.Y(ccD)
);
assign ccEn = ctrlWrCC | jmpDoRti;

//------------------------------------------------------------------------------
// Handle jump adder inputs/jump computations.
assign jmpAddA      = exePCQ;
assign jmpAddB      = immGenImm[15:1];
assign jmpAddI      = 1'b0;
Mux2 M10[14:0] (
	.A(jmpAddS),         // Conditional Jump? Use PC+Offset  (BRC)
	.B(aluResult[15:1]), // No?               Use Ptr+offset (JPR/JLR)
	.S(ctrlAllowJmp),
	.Y(jmpAddr)
);
assign jmpCodeMatch = (|(exeQ[11:9] & ccQ[3:1])) & (~exeQ[8] | ccQ[0]);
assign jmpDoJmp     = ctrlUseJmp                                        // JLR
                      | (ctrlAllowJmp & jmpCodeMatch)                   // BRC
							 | (ctrlIsJPR & ~exeQ[5]);                         // JPR
assign jmpDoRti     = ctrlIsJPR & exeQ[5] & intQ;                       // (JPR)

//------------------------------------------------------------------------------
// Handle memory controller inputs.
assign memIAddr = pcQ;
assign memDAddr = aluResult[15:1]; // word addressable access
assign memDData = regRData2;
assign memDEn   = ctrlIsMemInstr;
assign memDWr   = ctrlWrMem;
assign memDSwp  = ctrlIsSWP;

//------------------------------------------------------------------------------
// Handle interrupt inputs.
assign intDoInt = i_intEn & i_smIsBooted & ~pauseQ & ~intQ; // run app -> INT
assign intD     = intDoInt;                                 // "0" after sets
assign intEn    = intDoInt | jmpDoRti;                      // set and clear

//------------------------------------------------------------------------------
// Handle shadow register inputs.
Mux2 M11[14:0] (
	.A(jmpAddr),               // Jumping w/ interrupt? Save jump address
	.B(pcQ),                   // No?                   Save PC   address
	.S(jmpDoJmp),
	.Y(shadowPCD)
);
assign shadowPCEn = intDoInt;
Mux2 M12[3:0] (
	.A(aluCcodes),             // Computing w/ interrupt? Save computed codes
	.B(ccQ),                   // No?                     Save previous codes
	.S(ctrlWrCC),
	.Y(shadowCCD)
);
assign shadowCCEn = intDoInt;

//------------------------------------------------------------------------------
// Drive memory controls.
assign o_memAddr    = memCoreMemAddr;
assign o_memDataOut = memCoreMemData;
assign o_memWr      = memCoreMemWr;

//------------------------------------------------------------------------------
// Drive state machine outputs.
assign o_smNowPaused = pauseQ;

//------------------------------------------------------------------------------
// Drive reported values/signals.
assign o_reportPC  = pcQ;
assign o_reportSP  = regReportSP;
assign o_reportHLT = ctrlIsHLT;

endmodule
