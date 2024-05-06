/*
 * devProject.sv
 * 
 * "Top level of 'chasis' connecting processor design to development FPGA pins"
 */
module devProject (
    // Direct I/O Devices.
    input  [17:0] SW,
    input  [3:0]  KEY,
    output [17:0] LEDR,
    output [8:0]  LEDG,
    
    // 7-Segment Displays.
    output [6:0]  HEX0,
    output [6:0]  HEX1,
    output [6:0]  HEX2,
    output [6:0]  HEX3,
    output [6:0]  HEX4,
    output [6:0]  HEX5,
    output [6:0]  HEX6,
    output [6:0]  HEX7,
    
    // 50 MHz Clock Input.
    input CLOCK_50,
    
    // SRAM Signals.
    output [17:0] SRAM_ADDR,
    inout  [15:0] SRAM_DQ,
    output        SRAM_WE_N,
    output        SRAM_OE_N,
    output        SRAM_UB_N,
    output        SRAM_LB_N,
    output        SRAM_CE_N,
    
    // GPIO Ports.
    inout  [35:0] GPIO_0,
    inout  [35:0] GPIO_1
);

/////////////////////////////
// -- CHASIS Wires/Regs -- //
/////////////////////////////

// PLL block wires.
wire pllClockD, pllClockQ;
wire pllSelect;

// Reset wires/registers.
wire resetD;
reg  resetQ;

// Segment Display "Debug Words" wires.
wire[27:0] debugCtrl0, debugCtrl1;
wire[15:0] debugWord0, debugWord1;

///////////////////////////////////
// -- CHASIS Blocks/Instances -- //
///////////////////////////////////

// PLL Block- generates 8.333 MHz and 8 Hz clocks for development.
LazyPll PLL (
    .pll_clock50_i(pllClockD),
    .pll_switchFreq_i(pllSelect),
    .pll_genClk_o(pllClockQ)
);

// Reset signal- synchronized to generated clock signal.
always @(posedge pllClockQ) begin
    resetQ <= resetD;
end

// Segmen Displays- organized as two 16-bit words (for debugging).
SegDisplay DBG_WORD0[3:0] (
    .seg_nibble_i(debugWord0),
     .seg_controls_o(debugCtrl0)
);
SegDisplay DBG_WORD1[3:0] (
    .seg_nibble_i(debugWord1),
     .seg_controls_o(debugCtrl1)
);

/////////////////////////////////
// -- CHASIS Connects/Logic -- //
/////////////////////////////////

// Generate proper clock signal for DUT.
assign pllClockD = CLOCK_50;
assign pllSelect = ~KEY[1];  // Pushbutton 2nd from the right

// Generate/syncrhonize reset signal to generated clock signal.
assign resetD    = KEY[0];   // Rightmost Pushbutton

// Create feedback for PLL control.
assign LEDG[1] = ~KEY[1];   // Pushbutton/Green LED 2nd from the right
assign LEDG[2] = pllClockQ; // LED 3rd from the right

// Create feedback for Reset control.
assign LEDG[0] = resetQ;    // Rightmost GreenLED

// Create feedback for "debug words" for development.
assign HEX0 = debugCtrl0[6:0];
assign HEX1 = debugCtrl0[13:7];
assign HEX2 = debugCtrl0[20:14];
assign HEX3 = debugCtrl0[27:21];
assign HEX4 = debugCtrl1[6:0];
assign HEX5 = debugCtrl1[13:7];
assign HEX6 = debugCtrl1[20:14];
assign HEX7 = debugCtrl1[27:21];

///////////////////////////////////
// -- Device Under Test (DUT) -- //
///////////////////////////////////

/*
 * Resources available (from Chasis):
 * == Expected/Production Signals ==
 * -> pllClockQ             : Input Clock Signal
 * -> resetQ                : Input (Synchronous) Reset Signal
 * -> SRAM_<sigs>           : RAM chip signals
 * -> GPIO_0, GPIO_1        : Generic pins (for JTAG, SPI, etc)
 *
 * == Debug/Test Signals ==
 * -> debugWord0, debugWord1: words sent to hex displays
 */

// Microprocessor DUT Incarnate...
UProc DUT (
    // SRAM chip connections.
    .uproc_sramAddr(),
    .uproc_sramData(),
    .uproc_sramWr(),
    .uproc_sramEn(),
    
    // EEPROM SPI chip connections.
    .uproc_spiSCK(),
    .uproc_spiSDO(),
    .uproc_spiSDI(),
    .uproc_spiSCS(),
    
    // JTAG port connections.
    .uproc_jtagTCK(GPIO_0[0]), // "Upper" 3 pins of GPIO_0 connector
    .uproc_jtagTDI(GPIO_0[1]),
    .uproc_jtagTDO(GPIO_0[2]),
    .uproc_jtagTMS(GPIO_0[3]),
    
    // Common signals.
    .uproc_clk(pllClockQ),
    .uproc_rstn(resetQ),
    
    // TODO- test signals for development. TO DELETE!!!
    .test_word0(debugWord0),
    .test_word1(debugWord1)
);

//////////////////////////////////////
// -- Disabled/Unconnected Ports -- //
//////////////////////////////////////

// Direct I/O Devices.
assign LEDR      = 18'b0;
assign LEDG[8:3] = 6'b0;

// SRAM Signals.
assign SRAM_ADDR = 18'b0;
assign SRAM_DQ   = 16'bZZZZZZZZZZZZZZZZ;
assign SRAM_WE_N = 1'b1;
assign SRAM_OE_N = 1'b1;
assign SRAM_UB_N = 1'b1;
assign SRAM_LB_N = 1'b1;
assign SRAM_CE_N = 1'b1;

// GPIO Ports.
assign GPIO_0[35:4] = 32'bZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ;
assign GPIO_1 = 36'bZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ;

endmodule
