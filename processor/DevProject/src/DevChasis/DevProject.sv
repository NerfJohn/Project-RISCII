/*
 * DevProject.sv
 * 
 * "Top level of 'chasis' connecting processor design to development FPGA pins"
 */
module DevProject (
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
    input         CLOCK_50,
    
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

/*
 * DevProject Chasis + Microprocessor (UProc):
 *
 * Project/platform for developing Project RISCII microprocessor design. Project
 * includes both the core microprocessor deign (as the "device under test/DUT")
 * and a chasis to set it up on the FPGA. Below describes the FPGA controls.
 *
 * control name | location | defined with | desc.
 * -------------+----------+--------------+------
 * reset button | KEY[0]   | Chasis sigs  | External reset pin on MCU design
 * switch clks  | KEY[1]   | Chasis sigs  | Switch between 8.333 MHz and 8 Hz
 * reset LED    | LEDG[0]  | Chasis sigs  | visual confirmation of reset state
 * switch LED   | LEDG[1]  | Chasis sigs  | visual confirmation of switch press
 * clk LED      | LEDG[2]  | Chasis sigs  | visual confirmation of clk input
 * JTAG TCK LED | LEDG[3]  | DUT sigs     | visual check of JTAG's TCK
 * RAM En LED   | LEDG[4]  | DUT sigs     | visual check of RAM access
 * SPI EN LED   | LEDG[5]  | DUT sigs     | visual check of SPI access
 */

/////////////////////////////////////////////
// -- CHASIS Signals/Internal Registers -- //
/////////////////////////////////////////////

// PLL block wires.
wire pll_clkD, pll_clkQ;
wire pll_selPulse;

// Reset wires/registers.
wire resetD;
reg  resetQ;

// Segment Display "Debug Words" wires.
wire [27:0] seg_ctrl0, seg_ctrl1;
wire [15:0] seg_word0, seg_word1;

//////////////////////////////////////////////////////
// -- CHASIS Behavioral Blocks/Functional Instances //
//////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// PLL Block- generates 8.333 MHz and 8 Hz clocks for development.
LazyPll PLL (
    // Input 50 MHz clock.
    .i_clock50(pll_clkD),
    
    // Pulsing freq. selector.
    .i_switchFreq(pll_selPulse),
    
    // Generated clock.
    .o_genClk(pll_clkQ)
);

//------------------------------------------------------------------------------
// Reset signal- synchronized to generated clock signal.
always @(posedge pll_clkQ) begin
    resetQ <= resetD;
end

//------------------------------------------------------------------------------
// Segment Displays- organized as two 16-bit words (for debugging).
SegDisplay DBG_WORD0[3:0] (
    // Basic Encoder I/O.
    .i_segNibble(seg_word0),
    .o_segControls(seg_ctrl0)
);
SegDisplay DBG_WORD1[3:0] (
    // Basic Encoder I/O.
    .i_segNibble(seg_word1),
    .o_segControls(seg_ctrl1)
);

//////////////////////////////////////////////////
// -- CHASIS Connections/Combinational Logic -- //
//////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Generate proper clock signal for DUT.
assign pll_clkD     = CLOCK_50;
assign pll_selPulse = ~KEY[1];  // Pushbutton 2nd from the right

//------------------------------------------------------------------------------
// Generate/syncrhonize reset signal to generated clock signal.
assign resetD = KEY[0];   // Rightmost Pushbutton

//------------------------------------------------------------------------------
// Create feedback for PLL control.
assign LEDG[1] = ~KEY[1];   // Pushbutton/Green LED 2nd from the right
assign LEDG[2] = pll_clkQ;  // LED 3rd from the right

//------------------------------------------------------------------------------
// Create feedback for Reset control.
assign LEDG[0] = resetQ;    // Rightmost GreenLED

//------------------------------------------------------------------------------
// Create feedback for "debug words" for development.
assign HEX0 = seg_ctrl0[6:0];
assign HEX1 = seg_ctrl0[13:7];
assign HEX2 = seg_ctrl0[20:14];
assign HEX3 = seg_ctrl0[27:21];
assign HEX4 = seg_ctrl1[6:0];
assign HEX5 = seg_ctrl1[13:7];
assign HEX6 = seg_ctrl1[20:14];
assign HEX7 = seg_ctrl1[27:21];

////////////////////////////////////////////////////////////////////////////////
// -- Device Under Test (DUT) -- //
////////////////////////////////////////////////////////////////////////////////

/*
 * CHASIS Resources available:
 * == Expected/Production Signals ==
 * -> pll_clkQ             : Input Clock Signal
 * -> resetQ               : Input (Synchronous) Reset Signal
 * -> SRAM_<sigs>          : RAM chip signals
 * -> GPIO_0, GPIO_1       : Generic pins (for JTAG, SPI, etc)
 *
 * == Debug/Test Signals ==
 * -> seg_word0, seg_word1 : Words sent to hex displays
 */

//------------------------------------------------------------------------------
// Broken out SRAM signals (to handle specifc SRAM chip interface).
wire dut_sramWr, dut_sramEn;

//------------------------------------------------------------------------------
// Microprocessor DUT Incarnate...
UProc DUT (
    // Memory/SRAM chip connections (IO due to B-Scan).
    .io_memAddr(SRAM_ADDR),
    .io_memData(SRAM_DQ),
    .io_memWr(dut_sramWr),
    .io_memEn(dut_sramEn),

    // Storage/EEPROM SPI connections (IO due to B-Scan).
    .io_storeSCK(GPIO_0[29]),
    .io_storeSDI(GPIO_0[28]), // TODO- "data in"to slave- refactor naming...
    .io_storeSDO(GPIO_0[33]),
    .io_storeSCS(GPIO_0[32]),

    // GPIO connections.
    .io_gpioPin(),

    // State/Status connections (IO due to B-Scan).
    .io_isBooted(),
    .io_isPaused(),
    
    // JTAG port connections.
    .i_jtagTCK(GPIO_0[0]), // "Upper" 3 pins of GPIO_0 connector
    .i_jtagTDI(GPIO_0[1]),
    .o_jtagTDO(GPIO_0[2]),
    .i_jtagTMS(GPIO_0[3]),

    // Common signals.
    .i_clk(pll_clkQ),
    .i_rstn(resetQ),

    // TODO- Test signals for development; DELETE FOR PRODUCTION!!!
    .o_test_word0(seg_word0),
    .o_test_word1(seg_word1)
);

//------------------------------------------------------------------------------
// Connect to specific SRAM chip signals.
assign SRAM_WE_N = ~dut_sramWr;             // Inverted write/read signal
assign SRAM_OE_N = dut_sramWr;              // Allow output on read request
assign SRAM_UB_N = dut_sramWr & ~pll_clkQ;  // Write trigger- sync w/ clk
assign SRAM_LB_N = dut_sramWr & ~pll_clkQ;  // Write trigger- sync w/ clk
assign SRAM_CE_N = ~dut_sramEn;             // Inverted enable signal

//------------------------------------------------------------------------------
// Handle EEPROM specific signals.
assign GPIO_0[35] = 1'b0; // GND
assign GPIO_0[34] = 1'b1; // Write Protect disabled
assign GPIO_0[31] = 1'b1; // VDD
assign GPIO_0[30] = 1'b1; // Hold disabled

//------------------------------------------------------------------------------
// TODO- Additional feedback signals for development- TO DELETE!!!
assign LEDG[3] = GPIO_0[0];   // JTAG's TCK signal
assign LEDG[4] = dut_sramEn;  // RAM En signal
assign LEDG[5] = ~GPIO_0[32]; // EEPROM/SPI En signal

////////////////////////////////////////////////////////////////////////////////
// -- Disabled/Unconnected Ports -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Direct I/O Devices.
assign LEDR [17:0] = 16'b0;
assign LEDG [8:6]  = 3'b0;

//------------------------------------------------------------------------------
// SRAM Signals.
assign SRAM_ADDR[17:16] = 2'b0;

//------------------------------------------------------------------------------
// GPIO Ports.
assign GPIO_0 [27:4] = 24'bZZZZZZZZZZZZZZZZZZZZZZZZ;
assign GPIO_1 [35:0] = 36'bZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ;

endmodule
