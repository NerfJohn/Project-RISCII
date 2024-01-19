module AlteraProject(SW, HEX, KEY, LEDR, LEDG,
                     CLOCK_50
);

// Special inputs.
input CLOCK_50; // 50 MHz clock signal

// User inputs.
input[7:0] SW;  // Switches (left/righmost 4)- held high or low
input[3:0] KEY; // Active low pushbuttons

// User outputs.
output[55:0] HEX; // 7-segment displays- [HEX7[6]:HEX0[0]] ordering
output[3:0] LEDR; // Red LEDs
output[3:0] LEDG; // Green LEDs

/*
// Helper modules for ease of resources.
logic clkSignal;
LazyPll #(.DIVIDER(1000000)) iClk (.inClk(CLOCK_50), .outClk(clkSignal));
SegDisplay iDisplay[7:0] (.inNibble(32'b0), .outSegs(HEX));

assign LEDR = SW;
assign LEDG = {4{clkSignal}} & KEY;
*/

logic[1:0] state;

// Intermediate signals.
logic dataEn, dataWr, myRstn;
wire[15:0] dataBus;

// 50 Hz clk signal.
LazyPll #(.DIVIDER(1000000)) iClk (.inClk(CLOCK_50), .outClk(clkSignal));

// Example timer.
Timer16 iTMR (.busAddr(state), .busData(dataBus), .busEn(dataEn), .busWr(dataWr), 
					 .sigIntr(LEDR[3]), .clk(clkSignal), .rstn(myRstn)
);
					 
// Human input.
assign myRstn = KEY[0];
assign dataEn = ~KEY[1];
assign dataWr = ~KEY[2];
assign dataBus = (dataEn & ~dataWr) ? 16'bz : {10'b0, SW[5:0]};
assign state = SW[7:6];

// Human feedback.
assign LEDR[1:0] = state;
assign LEDR[2] = 0;
assign LEDG = ~KEY;
SegDisplay iDisplay[7:0] (.inNibble({10'b0, SW[5:0], dataBus}), .outSegs(HEX));

endmodule
