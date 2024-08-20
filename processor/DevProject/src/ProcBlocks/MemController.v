/*
 * MemController.v
 *
 * "Centralized circuit to control uP's memory bus"
 */
module MemController (
	// JTAG port connection.
	input  [15:0] i_jtagAddr,
	input  [15:0] i_jtagDataIn,
	input         i_jtagWr,
	input         i_jtagEn,
	
	// Selected/driven memory bus (contains tristated data line).
	output [15:0] o_memAddr,
	output        o_memWr,
	output        o_memEn,
	inout  [15:0] io_memData
);

/*
 * Controller determining which circuit is in control of the uP's memory bus.
 *
 * At present, only the JTAG can control the memory bus. As the uP is developed,
 * additional sources (and necessary muxing) will be added.
 *
 * Design Notes:
 * 1) This module serves as the lone "data line tristate" to ease FPGA use.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Data line tristate wires.
wire [15:0] triA, triY;
wire        triEn;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Data Line Tristate- centralized data bus driver for entire uP.
Tristate TRISTATE[15:0] (
	.A(triA),
	.Y(triY),
	.S(triEn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Handle driving of memory data lines.
assign triA  = i_jtagDataIn;
assign triEn = i_jtagWr;

//------------------------------------------------------------------------------
// Drive memory bus outputs.
assign o_memAddr  = i_jtagAddr;
assign o_memWr    = i_jtagWr;
assign o_memEn    = i_jtagEn;
assign io_memData = triY;

endmodule
