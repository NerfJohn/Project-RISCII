/*
 * CoreMem.v
 *
 * "Memory interface cordinating instruction vs data accesses"
 */
module CoreMem (
	// Access connections.
	input[14:0]  i_iAddr,
	input[14:0]  i_dAddr,
	input[15:0]  i_dData,
	
	// "Control" inputs (generally data driven).
	input        i_dEn,
	input        i_dWr,
	input        i_dSwp,
	
	// Status outputs (to core).
	output       o_dRead,
	output       o_dEnd,
	
	// Controller outputs (to rest of uP).
	output[15:0] o_coreMemAddr,
	output[15:0] o_coreMemData,
	output       o_coreMemWr,
	
	// Common signals.
	input        i_clk,
	input        i_rstn
);

/*
 * TODO- desc. Note: isSwp implies Rd/Wr; isWr can be either
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Interface's State Machine wires.
wire[1:0]  stateD, stateQ;
wire[1:0]  stateNew, stateNext;
wire       stateStartOp;

// Computed signals based on current state.
wire       inStateIdle;
wire       inStateWrite;

// Saved "data" for data memory access.
wire[14:0] saveAddrD, saveAddrQ;
wire[15:0] saveDataD, saveDataQ;

// Computed signals based on "data" inputs.
wire[14:0] addrLowBits;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Interface State Machine- controlling flow of memory accesses.
DffSynch STATE[1:0] (
	.D(stateD),
	.Q(stateQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Saved Data "Data"- saved here to prevent potential corruption on swap (SWP).
DffSynch SAVE_ADDR[14:0] (
	.D(saveAddrD),
	.Q(saveAddrQ),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynch SAVE_DATA[15:0] (
	.D(saveDataD),
	.Q(saveDataQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Assign next state of interface.
assign stateStartOp = i_dEn & inStateIdle;
assign stateNew     = {i_dSwp | ~i_dWr, i_dSwp | i_dWr};
assign stateNext    = {1'b0, &stateQ};
Mux2 M0[1:0] (
	.A(stateNew),     // Starting New Op? get op's starting state...
	.B(stateNext),    // No?              use usual state pathing
	.S(stateStartOp),
	.Y(stateD)
);

//------------------------------------------------------------------------------
// Compute state-related signals.
assign inStateIdle  = ~(|stateQ);             // IDLE  (00)
assign inStateWrite = ~stateQ[1] & stateQ[0]; // WRITE (01)

//------------------------------------------------------------------------------
// Temporarily save data's address/value (for next cycle use).
assign saveAddrD = i_dAddr;
assign saveDataD = i_dData;

//------------------------------------------------------------------------------
// Compute "data"-related signals.
Mux2 M1[14:0] (
	.A(i_iAddr),     // IDLE? get instruction...
	.B(saveAddrQ),   // No?   get data instead
	.S(inStateIdle),
	.Y(addrLowBits)
);

//------------------------------------------------------------------------------
// Handle status outputs to core.
assign o_dRead = stateQ[1];             // READ (10) and SWAP-READ  (11)
assign o_dEnd  = stateQ[1] ^ stateQ[0]; // READ (10) and WRITE      (01)

//------------------------------------------------------------------------------
// Handle controller outputs to uP.
assign o_coreMemAddr = {~inStateIdle, addrLowBits}; // Top bit accesses region
assign o_coreMemData = saveDataQ;
assign o_coreMemWr   = inStateWrite;

endmodule
