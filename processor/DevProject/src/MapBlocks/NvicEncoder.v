/*
 * NvicEncoder.v
 *
 * "Priority Encoder designed for uP's 1-11 interrupt signals"
 */
module NvicEncoder (
	// Decoded connections.
	input         i_in1,
	input         i_in2,
	input         i_in3,
	input         i_in4,
	input         i_in5,
	input         i_in6,
	input         i_in7,
	input         i_in8,
	input         i_in9,
	input         i_in10,
	input         i_in11,
	
	// Encoded connections.
	output [3:0]  o_encVal
);

/*
 * 1-11 priority encoder (highest numbers first). Intended for interrupt use.
 *
 * Encodes a 4-bit value equivalent to 1-11, based on which input signal is
 * active. If multiple are active, then the highest value one is encoded.
 * Circuit is fully combinational and responds to changes as such.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Computed wires (based on signal inputs).
wire        in001X, in010X, in011X, in100X, in101X;
wire        in01XX, in10XX;

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute input based wires.
assign in001X = i_in2  | i_in3;
assign in010X = i_in4  | i_in5;
assign in011X = i_in6  | i_in7;
assign in100X = i_in8  | i_in9;
assign in101X = i_in10 | i_in11;
assign in01XX = i_in4 | i_in5 | i_in6 | i_in7;
assign in10XX = i_in8 | i_in9 | i_in10 | i_in11;

//------------------------------------------------------------------------------
// Drive encoded value (bit for bit).
assign o_encVal[0] =   i_in11 | // set for 11, else...
                     (~i_in10 & // clr for 10, else...
							( i_in9  | // etc...
							(~i_in8  &
							( i_in7  |
							(~i_in6  &
							( i_in5  |
							(~i_in4  &
							( i_in3  |
							(~i_in2  &
							  i_in1
							)))))))));
assign o_encVal[1] =   in101X | // set for 10/11, else...
                     (~in100X & // clr for 8/9,   else...
							( in011X | // etc...
							(~in010X &
							  in001X
							)));
assign o_encVal[2] =  ~in10XX & // clr for 8/9/10/11, else...
                       in01XX   // set for 4/5/6/7
							;
assign o_encVal[3] =   i_in11 | // set for 1XXX
                       i_in10 |
							  i_in9  |
							  i_in8
							;

endmodule
