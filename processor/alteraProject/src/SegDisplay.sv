module SegDisplay(inNibble, outSegs);

// Inputs/Ouputs.
input[3:0] inNibble; // Input nibble (4 bits)
output[6:0] outSegs; // Segment signals for hex value

// Convert 4-bit number into (active-low) segment signals.
always @(inNibble) begin
    case (inNibble)
        4'h0:    outSegs = ~7'h3F;
        4'h1:    outSegs = ~7'h06;
        4'h2:    outSegs = ~7'h5B;
        4'h3:    outSegs = ~7'h4F;
        4'h4:    outSegs = ~7'h66;
        4'h5:    outSegs = ~7'h6D;
        4'h6:    outSegs = ~7'h7D;
        4'h7:    outSegs = ~7'h07;
        4'h8:    outSegs = ~7'h7F;
        4'h9:    outSegs = ~7'h6F;
        4'hA:    outSegs = ~7'h77;
        4'hB:    outSegs = ~7'h7C;
        4'hC:    outSegs = ~7'h39;
        4'hD:    outSegs = ~7'h5E;
        4'hE:    outSegs = ~7'h79;
        default: outSegs = ~7'h71; // Default to 4'hF
    endcase
end

endmodule
