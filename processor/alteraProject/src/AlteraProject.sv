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

//==============================================================================

// Intermediate signals (related to help modules).
logic CLK_IN;
logic[31:0] HEX_OUT;

// Intermediate signals.
logic inTCK, inTMS, inTDI, outTDO;
logic       inRSTN;
logic[2:0]  sigSTATE;
logic[7:0]  sigIREG;
logic[15:0] sigDREG;

logic[15:0] sigMAX;

logic[15:0] busAddr;
wire busWr, busEn;
wire[15:0] busData;


// Adjusted clock signal and Hex output modules.
LazyPll #(.DIVIDER(2000000)) iClk (.inClk(CLOCK_50), .outClk(CLK_IN));
SegDisplay iDisplay[7:0] (.inNibble(HEX_OUT), .outSegs(HEX));

// DUT.
JtagMemController iJ(.tck(inTCK), .tms(inTMS), .tdi(inTDI), .tdo(outTDO),
						 .addr(busAddr), .data(busData), .isWr(busWr), .memEn(busEn),
						 .clk(CLK_IN), .rstn(inRSTN),
						 .test_state(sigSTATE),
						 .test_instr(sigIREG),
						 .test_data(sigDREG)
);

// DUT #2.
Timer16 iT (.busEn(busEn), .busWr(busWr), .busAddr(busAddr[1:0]), .busData(busData), .sigIntr(/*NC*/), .clk(CLK_IN), .rstn(inRSTN),
				.sigMAX(sigMAX));

// Human Input.
assign inTCK = SW[2];
assign inTMS = SW[1];
assign inTDI = SW[0];
assign inRSTN = KEY[0];

// Human Feedback.
assign LEDR[0] = outTDO;
assign LEDR[1] = busEn;
assign LEDR[2] = busWr;
assign HEX_OUT = {5'b0, sigSTATE, sigIREG, sigMAX};
assign LEDG = ~KEY;

endmodule
