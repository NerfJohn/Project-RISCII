////////////////////////////////////////////////////////////////////////////////
// AlteraProject.sv: "Root/Chasis to connect boge circuits to physical FPGA"
// ~John Eslinger
////////////////////////////////////////////////////////////////////////////////

module AlteraProject(
	// Human/Direct Inputs.
	input[7:0]   SW,       // On/Off switches (4 on either side of row)
	input[3:0]   KEY,      // Push-buttons (active-low) 
	
	// Human/Direct Outputs.
	output[55:0] HEX,      // 7-segment displays (mainly to show hex values)
	output[3:0]  LEDR,     // Red LEDs
	output[3:0]  LEDG,     // Green LEDs
	
	// Human/Direct Bi-directionals.
	inout[11:0]   GPIO0,    // Basic GPIOs (left->right/up->down on board)
	
	// FPGA/board provided 50 MHz clock.
	input        CLOCK_50,  // Clock signal
	
	// FPGA/board provided SRAM chip (512 KB, word-addressable).
	output[17:0] SRAM_ADDR, // address
	inout[15:0]  SRAM_DQ,   // data- for read and write
	output       SRAM_WE_N, // write enable (active low)
	output       SRAM_OE_N, // output enable (active low)
	output       SRAM_UB_N, // upper byte mask
	output       SRAM_LB_N, // lower byte mask
	output       SRAM_CE_N
);

///////////////////////////////
// -- Chasis wires/blocks -- //
///////////////////////////////

// Wires to connect to PLL/Hex Display blocks.
wire[31:0] _TO_HEX;
wire       _FROM_CLK;

// Signal to treat as common "reset" signal.
wire       _FROM_RSTN;

// "PLL" block to divide provided clock to desired speed.
LazyPll #(.DIVIDER(6)) iClk (
	.inClk(CLOCK_50), 
	.outClk(_FROM_CLK)
);

// Converter from nibble to 7-segment controls (for hexadecimal value).
SegDisplay iDisplay[7:0] (
	.inNibble(_TO_HEX),
	.outSegs(HEX)
);

// Have KEY[0]/LEDG[0] (ie rightmost button/LED) control resetting.
assign _FROM_RSTN = KEY[0];
assign LEDG[0] = ~KEY[0];   // Light indicates active low reset

// Show clk speed on LEDG[1] (helps for slower speeds).
assign LEDG[1] = _FROM_CLK;

///////////////////////////////////////////////////////////////////
// -- Semi-DUT: Experimental JTAG Controller aids development -- //
///////////////////////////////////////////////////////////////////

// Test signals related to JTAG DUT.
logic[15:0] test_jtagData;
logic[7:0] test_jtagState;

// Wires connecting JTAG controller to other DUTs/blocks.
wire[15:0] jtagAddr, jtagData;
wire jtagWr, jtagEn;
wire jtagTck, jtagTdi, jtagTdo, jtagTms;
wire jtagSck, jtagSdi, jtagSdo, jtagSen;

// JTAG DUT instance.
JtagMemController CTRL(
	.addr(jtagAddr),
	.data(SRAM_DQ),
	.isWr(jtagWr),
	.memEn(jtagEn),
	
	.tck(jtagTck),
	.tms(jtagTms),
	.tdi(jtagTdi),
	.tdo(jtagTdo),
	
	.spiControl(jtagSen),
	.spiClk(jtagSck),
	.spiTdi(jtagSdi),
	.spiTdo(jtagSdo),
	
	.clk(_FROM_CLK),
	.rstn(_FROM_RSTN),
	
	.test_instr(test_jtagState),
	.test_data(test_jtagData)
);

// Get JTAG signals from GPIOs.
assign jtagTck = GPIO0[0];
assign jtagTdi = GPIO0[1];
assign jtagTms = GPIO0[2];
assign GPIO0[3] = jtagTdo;

// Use LEDG[3] to show when JTAG connection is active.
assign LEDG[3] = jtagTck;

////////////////////////////////////////////////
// -- DUT: Module under active development -- //
////////////////////////////////////////////////

// Attach JTAG DUT to SRAM chip on FPGA/board.
assign SRAM_ADDR = {2'b00, jtagAddr};
assign SRAM_WE_N = ~jtagWr;
assign SRAM_OE_N = jtagWr;
assign SRAM_UB_N = jtagWr & ~_FROM_CLK; // related to write trigger- sync w/ clk posedge
assign SRAM_LB_N = jtagWr & ~_FROM_CLK; // related to write trigger- sync w/ clk posedge
assign SRAM_CE_N = ~jtagEn;

// -- TODO- add this to a controller block for SPI pins. -- //
wire pinScs, pinSck, pinSmo, pinSmi; // Typical SPI signals
wire unlockSck;

// register to delay clk by 1 cycle (prevent JTAG enter data shift from being counted).
myDff CLK_GATE (.D(jtagSen), .Q(unlockSck), .clk(jtagSck), .rstn(_FROM_RSTN));

// Set up SPI pins.
assign pinScs = ~jtagSen & ~unlockSck;
assign pinSck = jtagSck & unlockSck;
assign pinSmo = jtagSdi & jtagSen;
assign jtagSdo = pinSmi;

// Tie to output leds for checking.
assign LEDR[3:0] = {pinSmi, pinSmo, pinSck, pinScs};
assign GPIO0[4] = pinScs;
assign pinSmi = GPIO0[5];
assign GPIO0[6] = 1'b1;
assign GPIO0[7] = 1'b0;
assign GPIO0[8] = pinSmo;
assign GPIO0[9] = pinSck;
assign GPIO0[10] = 1'b1;
assign GPIO0[11] = 1'b1;


/*
assign GPIO0[7:4] = {pinScs, pinSmi, 1'b1, 1'b0}; // SOIC pins {1, 2, 3, 4} (write protect OFF)
assign GPIO0[11:8] = {1'b1, 1'b0, pinSck, pinSmo}; // SOIC pins {8, 7, 6, 5} (hold OFF)
*/

// -- -------------------------------------------------- -- //

// Watch JTAGs data.
assign _TO_HEX = {8'h0, test_jtagState, test_jtagData};

endmodule
