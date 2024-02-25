module SynchFlop (
	// Typical flop signals.
	input  D,
	output Q,
	
	// Common signals.
	input clk,
	input rstn
);

/////////////////////////
// -- Signals/Wires -- //
/////////////////////////

// External-facing DFF wires.
wire synchExtD, synchExtQ;

// Internal-facing DFF wires.
wire synchInD, synchInQ;

////////////////////////////
// -- Blocks/Instances -- //
////////////////////////////

// External-facing DFF.
myDff SYNCH_EXT (.D(synchExtD), .Q(synchExtQ), .clk(clk), .rstn(rstn));

// Internal-facing DFF.
myDff SYNCH_IN (.D(synchInD), .Q(synchInQ), .clk(clk), .rstn(rstn));

//////////////////////////
// -- Connects/Logic -- //
//////////////////////////

// Input (typically from external pin).
assign synchExtD = D;

// Mid-synchroning signal.
assign synchInD = synchExtQ;

// Output synchronized signal.
assign Q = synchInQ;

endmodule
