module AlteraProject(SW, HEX, KEY, LEDR, LEDG, GPIO0,
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

// User Inouts.
inout[3:0] GPIO0;

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
wire[15:0] T_data, T_val, T_ctl, T_max, T_cmp;

// 50 Hz clk signal.
LazyPll #(.DIVIDER(1000000)) iClk (.inClk(CLOCK_50), .outClk(clkSignal));

// Example timer.
Uart iTMR (.busAddr(state), .busData(dataBus), .busEn(dataEn), .busWr(dataWr), 
					 .sigTxInt(LEDR[3]), .sigRxInt(/*NC*/), .txOut(GPIO0[0]), .rxIn(GPIO0[3]),
					 .clk(clkSignal), .rstn(myRstn),
					 .T_a0(T_val), .T_a1(T_ctl), .T_a2(T_max), .T_a3(T_cmp)
);
					 
// Human input.
assign myRstn = KEY[0];
assign dataEn = ~KEY[1];
assign dataWr = ~KEY[2];
assign dataBus = (dataEn & ~dataWr) ? 16'bz : {10'b0, SW[5:0]};
assign state = SW[7:6];

assign GPIO0[2:1] = 0;

// Human feedback.
assign LEDR[1:0] = state;
assign LEDR[2] = 0;
assign LEDG = ~KEY;
assign T_data = (state == 0) ? T_val :
                (state == 1) ? T_ctl :
					 (state == 2) ? T_max :
					 T_cmp;
SegDisplay iDisplay[7:0] (.inNibble({dataBus, T_data}), .outSegs(HEX));

endmodule
