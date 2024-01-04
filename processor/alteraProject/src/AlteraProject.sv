module AlteraProject(SW, HEX, KEY, LEDR, LEDG,
                     CLOCK_50
);

// Special inputs.
input CLOCK_50; // 50 MHz clock signal

// User inputs.
input[3:0] SW;  // Switches- held high or low
input[3:0] KEY; // Active low pushbuttons

// User outputs.
output[55:0] HEX; // 7-segment displays- [HEX7[6]:HEX0[0]] ordering
output[3:0] LEDR; // Red LEDs
output[3:0] LEDG; // Green LEDs

// Helper modules for ease of resources.
logic clkSignal;
LazyPll #(.DIVIDER(1000000)) iClk (.inClk(CLOCK_50), .outClk(clkSignal));
SegDisplay iDisplay[7:0] (.inNibble(32'b0), .outSegs(HEX));

assign LEDR = SW;
assign LEDG = {4{clkSignal}} & KEY;


endmodule
