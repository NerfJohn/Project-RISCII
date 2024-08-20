/*
 * BoundaryScan.v
 *
 * "Large "shift" register providing read only access to various interfaces"
 */
module BoundaryScan (
	// Shift connections.
	input         i_shiftIn,
	output        o_shiftOut,
	
	// Shift control.
	input         i_doShift,
	
	// Read connections.
	input  [15:0] i_memAddr,
	input  [15:0] i_memData,
	input         i_memWr,
	input         i_memEn,
	input         i_spiMISO,
	input         i_spiMOSI,
	input         i_spiCLK,
	input         i_spiCSn,
	input  [15:0] i_gpioPins,
	input         i_smDoPause,
	input         i_smIsBooted,
	input         i_smIsPaused,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * TODO- desc.
 * 1) Register are Asynch reset as clock is likely TCK, not sys clk
 * 2) Dummy registers added to make overall register byte aligned (ie 8 bytes)
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Sample register wires.
wire [56:0] sampleD, sampleQ;

// Shift register wires.
wire [56:0] shiftD, shiftQ;

// Dummy register wires.
wire [6:0]  dummyD, dummyQ;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Sample register- samples probed lines (acting as 1st half of synch).
DffAsynch SAMPLE[56:0] (
	.D(sampleD),
	.Q(sampleQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Shift register- passes shifted or sampled data (2nd half of synch).
DffAsynch SHIFT[56:0] (
	.D(shiftD),
	.Q(shiftQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Dummy register- shift only, used to align boundary scan to byte size.
DffAsynch DUMMY[6:0] (
	.D(dummyD),
	.Q(dummyQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Probe read lines.
assign sampleD = {i_smIsPaused, // [56]
                  i_smIsBooted, // [55]
						i_smDoPause,  // [54]
						i_gpioPins,   // [53:38]
						i_spiCSn,     // [37]
						i_spiCLK,     // [36]
						i_spiMOSI,    // [35]
						i_spiMISO,    // [34]
						i_memEn,      // [33]
						i_memWr,      // [32]
						i_memData,    // [31:0]
						i_memAddr};   // [15:0]

//------------------------------------------------------------------------------
// Determine which value (shift vs read) to pass on.
Mux2 M0[56:0] (
	.A({shiftQ[55:0], i_shiftIn}), // Do Shift? Pass shifted data
	.B(sampleQ),                   // No?       Pass sampled data
	.S(i_doShift),
	.Y(shiftD)
);

//------------------------------------------------------------------------------
// Manage dummy buffer to byte align scan register.
assign dummyD = {dummyQ[5:0], shiftQ[56]} & {7{i_doShift}}; // Shift vs read GND

//------------------------------------------------------------------------------
// Drive shift out.
assign o_shiftOut = dummyQ[6];

endmodule
