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
inout[7:0] GPIO0;

//==============================================================================

// Intermediate signals (related to help modules).
logic CLK_IN;
logic[31:0] HEX_OUT;

// Intermediate signals.
logic inTCK, inTMS, inTDI, outTDO;
logic       inRSTN;
logic[2:0]  sigSTATE;
logic[7:0]  sigINSTR;
logic[15:0] sigDATA;
logic[15:0] sigADDR;
logic[1:0]  sigSIGS;

wire mem_isWr, mem_memEn;
wire[15:0] mem_addr, mem_data;

logic[5:0] sigTRIS;
wire dut_pins, sdaLine, sigBYTE;

// Adjusted clock signal and Hex output modules.
LazyPll #(.DIVIDER(50)) iClk (.inClk(CLOCK_50), .outClk(CLK_IN));
SegDisplay iDisplay[7:0] (.inNibble(HEX_OUT), .outSegs(HEX));

// JTAG controller (for better debugging).
JtagMemController CTRL(
	.addr(mem_addr),
	.data(mem_data),
	.isWr(mem_isWr),
	.memEn(mem_memEn),
	
	.tck(inTCK),
	.tms(inTMS),
	.tdi(inTDI),
	.tdo(outTDO),
	
	.clk(CLK_IN),
	.rstn(inRSTN),
	
	.test_state(sigSTATE),
	.test_instr(sigINSTR),
	.test_data(sigDATA),
	.test_addr(sigADDR),
	.test_sigs(sigSIGS)
);

// DUT.
I2cController DUT (
	// Memory Bus Connections.
	.I2cController_busAddr(mem_addr[1:0]),
	.I2cController_busWr(mem_isWr),
	.I2cController_busEn(mem_memEn),
	.I2cController_busData(mem_data),
	
	// I2C pins/signals.
	.I2cController_SCL(dut_pins),
	.I2cController_SDA(sdaLine),
	
	.test_state(sigTRIS),
	.test_data(sigBYTE),
	
	// Common Nets/Signals.
	.clk(CLK_IN),
	.rstn(inRSTN)
);

// TODO-remove.

// Human Input.
assign inRSTN = KEY[0];

// "Link" IO.
assign inTCK = GPIO0[0];	// "top left" Altera pin, pin 2 on nano
assign inTDI = GPIO0[1];	// "top right" Altera pin, pin 3 on nano
assign inTMS = GPIO0[2];	// "low left" Altera pin, pin 4 on nano
assign GPIO0[3] = outTDO;	// "low right" Altera pin, pin 5 on nano

assign LEDR = GPIO0[3:0];

assign GPIO0[5:4] = 2'b10;
assign GPIO0[7:6] = {dut_pins, sdaLine};

// Human Feedback.
assign HEX_OUT = {7'h0, sigTRIS[5], 8'h0,3'h0, sigTRIS[4:0], 3'h0, sigBYTE, 3'h0, dut_pins};
assign LEDG = ~KEY;

endmodule
