/*
 * SegDisplay.sv
 *
 * "Encoder turning 4 bits into hexadecimal signals for 7 segment display"
 */
module SegDisplay (
    // Basic Encoder I/O.
     input  [3:0] i_segNibble,
     output [6:0] o_segControls
);

//////////////////////////////////
// -- Behavioral Block Logic -- //
//////////////////////////////////

//------------------------------------------------------------------------------
// Convert 4-bit number into (active-low) segment signals.
always @(i_segNibble) begin
    case (i_segNibble)
        4'h0:    o_segControls = ~7'h3F;
        4'h1:    o_segControls = ~7'h06;
        4'h2:    o_segControls = ~7'h5B;
        4'h3:    o_segControls = ~7'h4F;
        4'h4:    o_segControls = ~7'h66;
        4'h5:    o_segControls = ~7'h6D;
        4'h6:    o_segControls = ~7'h7D;
        4'h7:    o_segControls = ~7'h07;
        4'h8:    o_segControls = ~7'h7F;
        4'h9:    o_segControls = ~7'h6F;
        4'hA:    o_segControls = ~7'h77;
        4'hB:    o_segControls = ~7'h7C;
        4'hC:    o_segControls = ~7'h39;
        4'hD:    o_segControls = ~7'h5E;
        4'hE:    o_segControls = ~7'h79;
        default: o_segControls = ~7'h71; // Default to 4'hF
    endcase
end

endmodule
 