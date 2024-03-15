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
logic[7:0]  sigINSTR;
logic[15:0] sigDATA;
logic[15:0] sigADDR;
logic[1:0]  sigSIGS;

wire mem_isWr, mem_memEn;
wire[15:0] mem_addr, mem_data;

logic[15:0] sigTRIS;
wire[15:0] dut_pins;

// Adjusted clock signal and Hex output modules.
LazyPll #(.DIVIDER(1000000)) iClk (.inClk(CLOCK_50), .outClk(CLK_IN));
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
GpioPort DUT (
	// Memory Bus Connections.
	.GPort_busAddr(mem_addr[1:0]),
	.GPort_busWr(mem_isWr),
	.GPort_busEn(mem_memEn),
	.GPort_busData(mem_data),
	
	// External Pin Connections.
	.GPort_extPin(dut_pins),
	
	.test_tris(sigTRIS),
	
	// Common Nets/Signals.
	.clk(CLK_IN),
	.rstn(inRSTN)
);

// TODO-remove.
assign dut_pins = (sigTRIS == 16'h0) ? {SW, SW} : 16'hzzzz;

// Human Input.
assign inRSTN = KEY[0];

// "Link" IO.
assign inTCK = GPIO0[0];	// "top left" Altera pin, pin 2 on nano
assign inTDI = GPIO0[1];	// "top right" Altera pin, pin 3 on nano
assign inTMS = GPIO0[2];	// "low left" Altera pin, pin 4 on nano
assign GPIO0[3] = outTDO;	// "low right" Altera pin, pin 5 on nano

assign LEDR = GPIO0;

// Human Feedback.
assign HEX_OUT = {dut_pins, sigDATA};
assign LEDG = ~KEY;

endmodule
