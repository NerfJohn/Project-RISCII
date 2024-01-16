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

/*
// Helper modules for ease of resources.
logic clkSignal;
LazyPll #(.DIVIDER(1000000)) iClk (.inClk(CLOCK_50), .outClk(clkSignal));
SegDisplay iDisplay[7:0] (.inNibble(32'b0), .outSegs(HEX));

assign LEDR = SW;
assign LEDG = {4{clkSignal}} & KEY;
*/

// Intermediate signals.
logic dataEn, dataWr, myRstn;
wire[15:0] dataBus, _testCnt;

// 50 Hz clk signal.
LazyPll #(.DIVIDER(100000)) iClk (.inClk(CLOCK_50), .outClk(clkSignal));

// Example timer.
timer_16b iTMR (.busAddr(2'b0), .busData(dataBus), .busEn(dataEn), .busWr(dataWr), 
					 .sigIntr(/*NC*/), .clk2(1'b0), .clk(clkSignal), .rstn(myRstn),
					 .TEST_cnt(_testCnt), .TEST_ctrl(/*NC*/));
					 
// Human input.
assign myRstn = KEY[0];
assign dataEn = ~KEY[1];
assign dataWr = ~KEY[2];
assign dataBus = (dataEn & ~dataWr) ? 16'bz : {4{SW}};

// Human feedback.
assign LEDR = SW;
assign LEDG = ~KEY;
SegDisplay iDisplay[7:0] (.inNibble({dataBus, _testCnt}), .outSegs(HEX));

endmodule
