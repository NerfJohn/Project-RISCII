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
 *
 * TODO- redo
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Drive core's memory controls.
assign o_coreAddr = i_iAddr;
assign o_coreWr   = 1'b0;

endmodule
