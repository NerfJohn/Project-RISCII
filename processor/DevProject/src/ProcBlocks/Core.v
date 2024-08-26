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
wire        ctrlIsHLT;

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
	.o_isHLT(ctrlIsHLT)
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
assign o_reportSP  = exeQ;
assign o_reportHLT = ctrlIsHLT;

endmodule
