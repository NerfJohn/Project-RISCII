/*
 * CoreMemController.v
 *
 * "Memory controller specific to core's instruction/data memory pipelines"
 */
module CoreMemController (
    // Instruction memory inputs (implied enable/read- also word addressable).
    input  [14:0] i_iAddr,
	 
	 // Instruction memory status signals.
	 output        o_iIsBusy,
	 
	 // Data memory inputs.
	 input  [14:0] i_dAddr,
	 input  [15:0] i_dData,
	 input         i_dWr,
	 input         i_dSwp,
	 input         i_dEn,
	 
	 // Data memory status signals.
	 output        o_dIsReading,
	 output        o_dIsDone,
    
    // Unified memory interface (data for writing, implied enable).
    output [15:0] o_coreAddr,
    inout  [15:0] io_coreData,
    output        o_coreWr,

	 // Common signals.
	 input         i_clk,
	 input         i_rstn
);

/*
 * Core Memory Controller- local controller for instruction/data accesses.
 *
 * Interleaves instruction/data memory accesses for core processor. Implements
 * a minor state machine and status signals to work with core processor to
 * implement read, write, and swap instruction capabilities.
 *
 * == Internal State Machine ==
 * state | operation         | next state
 * ------+-------------------+-----------
 * 2'b00 | instruction read  | depends on data inputs
 * 2'b01 | data (swap) write | instruction read
 * 2'b10 | data swap read    | data (swap) write
 * 2'b11 | data read         | instruction read
 *
 * Design Notes:
 * 1) Can drive data line ONLY in "data write" state
 * 2) Data inputs should be held constant until "o_dIsDone" asserts
 * 3) User needs to coordinate with status signals for intended effect
 * 4) Pauses should be done when in "instruction read" state
 * 5) For reads, raw data line passed to "io_coreData" should be used
 * 6) For swaps, the "Wr" input MUST be 0 (ie swap acts as read)
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Wires related to state machine + nextState logic.
wire [1:0] stateD,     stateQ;
wire       inIState;
wire [1:0] fromIState, fromDState;

// Computed signals to control the core memory connection.
wire servingData, readingData, writingData, finishData;

// Wires related to SW to HW address translation.
wire [14:0] wordAddr;

// Wires related to buffer to hold addr/data.
wire [14:0] aBufferD, aBufferQ;
wire        aBufferEn;
wire [15:0] dBufferD, dBufferQ;
wire        dBufferEn;

// Wires related to tristate allowing for data memory writes.
wire [15:0] triA, triY;
wire        triEn;

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------
// State machine- coordinating instruction vs data access.
DffSynch STATE[1:0] (
	.D(stateD),
	.Q(stateQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Write buffer- ensures swap's address/write data won't change.
DffSynchEn ADDR_BUFFER[14:0] (
	.D(aBufferD),
	.Q(aBufferQ),
	.S(aBufferEn),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn DATA_BUFFER[15:0] (
	.D(dBufferD),
	.Q(dBufferQ),
	.S(dBufferEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Tristate for driving writes to memory.
Tristate WR_TRI[15:0] (
	.A(triA),
	.Y(triY),
	.S(triEn)
);

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Determine next state and update state machine.
assign inIState   = ~stateQ[1] & ~stateQ[0];        // serving instr mem
assign fromIState = {~i_dWr, ~i_dSwp} & {2{i_dEn}}; // state after instr serve
assign fromDState = {1'b0, stateQ[1] & ~stateQ[0]}; // state after data serve
Mux2 M0[1:0] (
	.A(fromIState), // switches to data state
	.B(fromDState), // switches to data or instruction state
	.S(inIState),
	.Y(stateD)
);

//------------------------------------------------------------------------------
// Compute internal control signals for memory connection.
assign servingData = stateQ[1] | stateQ[0];
assign readingData = stateQ[1];
assign writingData = ~stateQ[1] & stateQ[0];
assign finishData  = stateQ[0];

//------------------------------------------------------------------------------
// Handle driving memory bus' controls (address and read/write bit).
assign aBufferD   = i_dAddr;
assign aBufferEn  = i_dEn;                   // LDR/SWP breaks without this line
Mux2 M1[14:0] (
	.A(aBufferQ),
	.B(i_iAddr),
	.S(servingData),
	.Y(wordAddr)
);
assign o_coreAddr = {servingData, wordAddr}; // address proper section on chip
assign o_coreWr   = writingData;             // instructions always read

//------------------------------------------------------------------------------
// Handle driving/buffering of write data through data lines.
assign dBufferD    = i_dData;
assign dBufferEn   = i_dEn;        // LDR/SWP breaks without this line
assign triA        = dBufferQ;
assign triEn       = writingData;
assign io_coreData = triY;

//------------------------------------------------------------------------------
// Handle status signals for both instruction and data memory.
assign o_iIsBusy    = servingData;
assign o_dIsReading = readingData;
assign o_dIsDone    = finishData;

endmodule
