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
	
	// Mapped read connection.
	input  [15:0] i_mapReadData,
	
	// Selected/driven memory bus (contains tristated data line).
	output [15:0] o_memAddr,
	output        o_memRunWr,
	output        o_memRunEn,
	output        o_memMapWrEn,
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
 
// TODO- "1 tick" protection for JTAG write (eg write byte on fast UART).
// TODO- map protection when booting.

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Computed control wires (based on finalized mem controls).
wire        isMapAddr;
wire        isMapRead;

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
// Compute finalized mem control based controls.
assign isMapAddr = i_jtagAddr[15] & i_jtagAddr[14];
assign isMapRead = isMapAddr & ~i_jtagWr & i_jtagEn;

//------------------------------------------------------------------------------
// Handle driving of memory data lines.
Mux2 M0[15:0] (
	.A(i_mapReadData), // Reading Map? Drive read map data
	.B(i_jtagDataIn),  // No?          Drive sourced write data
	.S(isMapRead),
	.Y(triA)
);
assign triEn = i_jtagWr | isMapRead;

//------------------------------------------------------------------------------
// Drive memory bus outputs.
assign o_memAddr    = i_jtagAddr;
assign o_memRunWr   = i_jtagWr;
assign o_memRunEn   = i_jtagEn & ~isMapAddr;
assign o_memMapWrEn = i_jtagWr & i_jtagEn & isMapAddr;
assign io_memData   = triY;

endmodule
