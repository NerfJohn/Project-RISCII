/*
 * BScanOut.v
 *
 * "Boundary scan shift register for 1 output-only pin"
 */
module BScanOut  (
	// Common "shift register" signals.
	input  D,
	output Q,
	input  S,
	
	// Pin signal- pass through for potential override.
	input  int_pin,
	output ext_pin,
	
	// Control signal permitting override to occur.
	input  allowOverride,
	
	// Common signals.
	input  clk,
	input  rstn
);

/*
 * BScanOut (Boundary Scan Output):
 *
 * Implements boundary scan register for 1 output-only pin. Acting like a 3-bit
 * shift register, module give user read/write/drive capabilties. Below
 * describes layout/controls of design.
 *
 * setting  | desc.
 * ---------+------
 * output   | drives pin to set level if tristate (and override) are enabled
 * tristate | allows for driving pin level directly
 * input    | reads external pin value
 *
 * Shift Order: D -> INP -> TRI -> OUT -> Q
 *
 * Design Notes:
 * 1) Everything, from shifting to sampling, is on same clock (likely TCK)
 * 2) Disables driving on shifting or lack of allowance (ie MCU paused)
 * 3) Tristate is arguably redundant for write only pin, but nevertheless...
 */

/////////////////////////
// -- Signals/Wires -- //
/////////////////////////

// Computed control wires.
wire overridePin;

// Determined pin value wire.
wire newPinValue;

// Built-in "Synch" wires.
wire synchD, synchQ;

// Setting registers' wires.
wire outD, outQ;
wire triD, triQ;
wire inpD, inpQ;

////////////////////////////
// -- Blocks/Instances -- //
////////////////////////////

// Half-synchronizer (other half implemented as INP_REG) for reading.
MyDff SYNCH_REG (
	.D(synchD),
	.Q(synchQ),
	.clk(clk),
	.rstn(rstn)
);

// Settings registers- all are used for a boundary scan output register.
DffEn OUT_REG (
	.D(outD),
	.Q(outQ),
	.S(S),
	.clk(clk),
	.rstn(rstn)
);
DffEn TRI_REG (
	.D(triD),
	.Q(triQ),
	.S(S),
	.clk(clk),
	.rstn(rstn)
);
MyDff INP_REG (
	.D(inpD),
	.Q(inpQ),
	.clk(clk),
	.rstn(rstn)
);

//////////////////////////
// -- Connects/Logic -- //
//////////////////////////

// Compute control signals.
assign overridePin = allowOverride & ~S & triQ;

// Determine pin driver.
Mux2 M0 (
	.A(outQ),
	.B(int_pin),
	.S(overridePin),
	.Y(newPinValue)
);

// Sample + synchronize pin.
assign synchD = newPinValue;

// Implement "shift" behavior of register (inp -> tri -> out).
Mux2 M0 (    // sample vs. shift
	.A(D),
	.B(synchQ),
	.S(S),
	.Y(inpD)
);
assign triD = inpQ;
assign outD = triQ;
assign Q    = outQ;

// Output selected driver/value.
assign ext_pin = newPinValue;

endmodule
