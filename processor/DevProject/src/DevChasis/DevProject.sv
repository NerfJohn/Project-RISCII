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
 * "PCB" Controls:
 * control name | location   | desc.
 * -------------|------------|------
 * reset pin    | SW[0]      | connection to uP's reset pin
 * pause pin    | SW[1]      | connection to uP's "doPause" pin
 * boot state   | LEDR[0]    | connection to uP's "isBooted" pin
 * pause state  | LEDR[1]    | connection to uP's "isPaused" pin
 *
 * "Special" Controls:
 * control name | location | desc.
 * -------------|----------|------
 * switch freq  | KEY[0]   | debug control to switch DUT's input clk frequency
 * clk freq     | LEDG[0]  | value/frequency of DUT's input clk signal
 * RAM enable   | LEDG[7]  | value of enable signal to RAM chip
 * SPI enable   | LEDG[6]  | value of enable signal to EEPROM chip
 * JTAG clk     | LEDG[5]  | value/frequency of DUT's input TCK signal
 * RAM address  | HEX4-7   | value output from DUT's RAM address pins
 * RAM value    | HEX0-3   | value in/output to/from DUT's RAM data pins
 */

////////////////////////////////////////////////////////////////////////////////
// -- DEV_CHASIS Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Lazy PLL wires.
wire pllClkD, pllClkQ;
wire pllSwitchFreq;

// 16-bit Segment Display wires.
wire [27:0] segLeftCtrl, segRightCtrl;
wire [15:0] segLeftWord, segRightWord;

// DUT connector wires (sans bidirectionals).
wire [15:0] dutMemAddr;
wire        dutMemEn;
wire        dutSpiCSn;
wire        dutJtagTCK;

////////////////////////////////////////////////////////////////////////////////
// -- DEV_CHASIS Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// PLL Block- generates 8.333 MHz and 4 Hz clocks for development.
LazyPll PLL (
    // Input 50 MHz clock.
    .i_clock50(pllClkD),
    
    // Pulsing freq. selector.
    .i_switchFreq(pllSwitchFreq),
    
    // Generated clock.
    .o_genClk(pllClkQ)
);

//------------------------------------------------------------------------------
// Segment Displays- organized as two 16-bit words (for debugging).
SegDisplay DBG_LEFT_WORD[3:0] (
    // Basic Encoder I/O.
    .i_segNibble(segLeftWord),
    .o_segControls(segLeftCtrl)
);
SegDisplay DBG_RIGHT_WORD[3:0] (
    // Basic Encoder I/O.
    .i_segNibble(segRightWord),
    .o_segControls(segRightCtrl)
);

////////////////////////////////////////////////////////////////////////////////
// -- DEV_CHASIS Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Generate desired clock signal for DUT.
assign pllClkD       = CLOCK_50;
assign pllSwitchFreq = ~KEY[0];
assign LEDG[0]       = pllClkQ;  // clk freq feedback

//------------------------------------------------------------------------------
// Display "special/debugging" data from DUT.
assign segLeftWord  = dutMemAddr;  // RAM address line
assign HEX4 = segLeftCtrl[6:0];
assign HEX5 = segLeftCtrl[13:7];
assign HEX6 = segLeftCtrl[20:14];
assign HEX7 = segLeftCtrl[27:21];
assign segRightWord = SRAM_DQ;     // RAM data line
assign HEX0 = segRightCtrl[6:0];
assign HEX1 = segRightCtrl[13:7];
assign HEX2 = segRightCtrl[20:14];
assign HEX3 = segRightCtrl[27:21];
assign LEDG[7]      = dutMemEn;    // RAM enable
assign LEDG[6]      = dutSpiCSn;   // SPI enable
assign LEDG[5]      = dutJtagTCK;  // JTAG clock

//------------------------------------------------------------------------------
// Connect DUT to external pins.

// TODO- implement.

//------------------------------------------------------------------------------
// ("Disconnect" unused FPGA output interface components.)
assign LEDR[17:2]       = 16'b0000000000000000;
assign LEDG[8]          = 1'b0;
assign LEDG[4:1]        = 4'b0000;
assign SRAM_ADDR[17:16] = 2'b00;
assign SRAM_WE_N        = 1'b1;
assign SRAM_OE_N        = 1'b1;
assign SRAM_UB_N        = 1'b1;
assign SRAM_LB_N        = 1'b1;
assign SRAM_CE_N        = 1'b1;
assign GPIO_0[35:0]     = 36'bZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ;
assign GPIO_1[35:0]     = 36'bZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ;

////////////////////////////////////////////////////////////////////////////////
// -- DUT_INSTANCE Wires/Blocks/Logic -- //
////////////////////////////////////////////////////////////////////////////////

// TODO- implement.
assign SRAM_DQ    = 16'bZZZZZZZZZZZZZZZZ;
assign dutMemAddr = 16'b0000000000000000;
assign dutMemEn   = 1'b0;
assign dutSpiCSn  = 1'b1;
assign dutJtagTCK = 1'b0;
assign LEDR[1:0]  = 2'b00;
assign SRAM_ADDR  = 16'b0000000000000000;

endmodule
