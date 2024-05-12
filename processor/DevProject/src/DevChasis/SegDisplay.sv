/*
 * SegDisplay.sv
 *
 * "Encoder turning 4 bits into hexadecimal signals for 7 segment display"
 */
module SegDisplay (
    // Basic Encoder I/O.
     input  [3:0] seg_nibble_i,
     output [6:0] seg_controls_o
);

//////////////////////////////
// -- Blocks/Connections -- //
//////////////////////////////

// Convert 4-bit number into (active-low) segment signals.
always @(seg_nibble_i) begin
    case (seg_nibble_i)
        4'h0:    seg_controls_o = ~7'h3F;
        4'h1:    seg_controls_o = ~7'h06;
        4'h2:    seg_controls_o = ~7'h5B;
        4'h3:    seg_controls_o = ~7'h4F;
        4'h4:    seg_controls_o = ~7'h66;
        4'h5:    seg_controls_o = ~7'h6D;
        4'h6:    seg_controls_o = ~7'h7D;
        4'h7:    seg_controls_o = ~7'h07;
        4'h8:    seg_controls_o = ~7'h7F;
        4'h9:    seg_controls_o = ~7'h6F;
        4'hA:    seg_controls_o = ~7'h77;
        4'hB:    seg_controls_o = ~7'h7C;
        4'hC:    seg_controls_o = ~7'h39;
        4'hD:    seg_controls_o = ~7'h5E;
        4'hE:    seg_controls_o = ~7'h79;
        default: seg_controls_o = ~7'h71; // Default to 4'hF
    endcase
end

endmodule
 