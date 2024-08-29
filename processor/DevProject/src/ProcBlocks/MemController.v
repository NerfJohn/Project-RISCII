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
	
	// Bootloader connection.
	input  [15:0] i_bootMemAddr,
	input  [15:0] i_bootDataIn,
	input         i_bootMemEn,
	
	// Core connection.
	input  [15:0] i_coreMemAddr,
	input  [15:0] i_coreMemDataIn,
	input         i_coreMemWr,
	
	// uP State connection.
	input         i_smIsPaused,
	input         i_smIsBooted,
	
	// Mapped read connection.
	input  [15:0] i_mapReadData,
	
	// Selected/driven memory bus (contains tristated data line).
	output [15:0] o_memAddr,
	output        o_memRunWr,
	output        o_memRunEn,
	output        o_memMapWrEn,
	inout  [15:0] io_memData,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * Controller determining which circuit is in control of the uP's memory bus.
 *
 * At present, only the JTAG and bootloader can control the memory bus. For the
 * bootloader, a write lock is placed on the mapped registers in case of a
 * corrupted binary image.
 *
 * Design Notes:
 * 1) This module serves as the lone "data line tristate" to ease FPGA use.
 */
 
// TODO- note "read/write on fall edge" stuff- enforces op on 2nd half ONLY

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Computed controls (based on potential controllers).
wire [15:0] curBootedAddr, curBootedDataOut;
wire        curBootedWr, curBootedEn;
wire [15:0] curAddr, curDataOut;
wire        curWr, curEn;

// Update synch wires.
wire [15:0] addrD, addrQ;
wire [15:0] dataD, dataQ;
wire        wrD, wrQ;
wire        enD, enQ;
wire        updateClk;

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
// Synch wires- ensures wr/en signals update in a manner preventing races.
DffSynch ADDR[15:0] (
	.D(addrD),
	.Q(addrQ),
	.clk(updateClk),  // custom clock behavior
	.rstn(i_rstn)
);
DffSynch DATA[15:0] (
	.D(dataD),
	.Q(dataQ),
	.clk(updateClk),  // custom clock behavior
	.rstn(i_rstn)
);
DffSynch WR (
	.D(wrD),
	.Q(wrQ),
	.clk(updateClk),  // custom clock behavior
	.rstn(i_rstn)
);
DffSynch EN (
	.D(enD),
	.Q(enQ),
	.clk(updateClk),  // custom clock behavior
	.rstn(i_rstn)
);

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
// Compute controller/uP state based controls.
Mux2 M0[15:0] (
	.A(i_jtagAddr),       // Paused? use JTAG address
	.B(i_coreMemAddr),    // No?     use core address
	.S(i_smIsPaused),
	.Y(curBootedAddr)
);
Mux2 M1[15:0] (
	.A(curBootedAddr),    // Booted? process booted addresses
	.B(i_bootMemAddr),    // No?     use bootloader address
	.S(i_smIsBooted),
	.Y(curAddr)
);
Mux2 M2[15:0] (
	.A(i_jtagDataIn),     // Paused? use JTAG write data
	.B(i_coreMemDataIn),  // No?     use core write data
	.S(i_smIsPaused),
	.Y(curBootedDataOut)
);
Mux2 M3[15:0] (
	.A(curBootedDataOut), // Booted? process booted write data
	.B(i_bootDataIn),     // No?     use bootloader write data
	.S(i_smIsBooted),
	.Y(curDataOut)
);
Mux2 M4 (
	.A(i_jtagWr),         // Paused? use JTAG read/write
	.B(i_coreMemWr),      // No?     use core read/write
	.S(i_smIsPaused),
	.Y(curBootedWr)
);
Mux2 M5 (
	.A(curBootedWr),      // Booted? process booted read/write
	.B(1'b1),             // No?     bootloader ALWAYS writes
	.S(i_smIsBooted),
	.Y(curWr)
);
Mux2 M6 (
	.A(i_jtagEn),         // Paused? use JTAG enable
	.B(1'b1),             // No?     core ALWAYS enabled
	.S(i_smIsPaused),
	.Y(curBootedEn)
);
Mux2 M7 (
	.A(curBootedEn),      // Booted? process booted enables
	.B(i_bootMemEn),      // No?     use bootloader enable
	.S(i_smIsBooted),
	.Y(curEn)
);

//------------------------------------------------------------------------------
// Handle update synch inputs.
assign addrD     = curAddr;
assign dataD     = curDataOut;
assign wrD       = curWr;
assign enD       = curEn;
assign updateClk = ~i_clk; // toggle enable opposite of selected clock

//------------------------------------------------------------------------------
// Compute finalized mem control based controls.
assign isMapAddr = curAddr[15] & curAddr[14];
assign isMapRead = isMapAddr & ~wrQ & enQ;

//------------------------------------------------------------------------------
// Handle driving of memory data lines.
Mux2 M8[15:0] (
	.A(i_mapReadData), // Reading Map? Drive read map data
	.B(dataQ),         // No?          Drive sourced write data
	.S(isMapRead),
	.Y(triA)
);
assign triEn = wrQ | isMapRead;

//------------------------------------------------------------------------------
// Drive memory bus outputs.
assign o_memAddr    = addrQ;
assign o_memRunWr   = wrQ;
assign o_memRunEn   = enQ & ~isMapAddr;
assign o_memMapWrEn = wrQ & enQ & isMapAddr & i_smIsBooted;
assign io_memData   = triY;

endmodule
