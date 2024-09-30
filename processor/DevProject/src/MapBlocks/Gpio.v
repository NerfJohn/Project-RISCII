/*
 * Gpio.v
 *
 * "Memory mapped controls for GPIO digital pins w/ alternate functions"
 */
module Gpio (
	// Memory Map connections.
	input  [1:0]  i_memAddr,
	input  [15:0] i_memDataIn,
	input         i_memWrEn,
	output [15:0] o_memDataOut,
	
	// Interrupt signal connections.
	output        o_intEXH,
	output        o_intEXL,
	
	// Alternate pin connections.
	input         i_pwmTmr2,
	input         i_pwmTmr3,
	input         i_uartTX,
	input         i_i2cSCLDir,
	input         i_i2cSDADir,
	output        o_uartRX,
	output        o_i2cSDAOut,
	
	// Raw pinout to outside uP.
	inout  [15:0] io_gpioPins,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * Port of generic digital IO pins. All configurable, some with alt functions.
 *
 * Each pin uses tristate controls (DIR and OUT) to control read vs write (read
 * register always reads regardless). Alternate functions override some/all of
 * tristate controls for given pins.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Computed controls (based on mem address).
wire        isCfgAddr, isDirAddr, isOutAddr;

// Config register wires.
wire [5:0]  cfgD, cfgQ;
wire        cfgEn;
wire        cfgUart, cfgI2C, cfgTMR2, cfgTMR3, cfgIntH, cfgIntL;

// Synchronized read wires.
wire [15:0] inpA, inpY;

// Direction register wires.
wire [15:0] dirD, dirQ;
wire        dirEn;

// Output register wires.
wire [15:0] outD, outQ;
wire        outEn;

// Edge detect wires.
wire        edgeHD, edgeHQ;
wire        edgeLD, edgeLQ;

// Interrupt wires.
wire        intHD, intHQ;
wire        intLD, intLQ;

// Tristate wires.
wire        curBit10, curBit11, curBit15;
wire        curDir12, curDir13;
wire [15:0] triA, triY, triEn;

// Compute data wires (based on read registers).
wire [15:0] readCfgReg;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Config Register- controls how pins are driven (and interrupt pins' config).
DffSynchEn CFG_REG[5:0] (
	.D(cfgD),
	.Q(cfgQ),
	.S(cfgEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Sychronized Read- reads all pin inputs, synched and stable.
ClkSynch INP_REG[15:0] (
	.A(inpA),
	.Y(inpY),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Direction Register- controls whether "Output" values are driven per pin.
DffSynchEn DIR_REG[15:0] (
	.D(dirD),
	.Q(dirQ),
	.S(dirEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Output Register- Values to set pins when uP is driving.
DffSynchEn OUT_REG[15:0] (
	.D(outD),
	.Q(outQ),
	.S(outEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Edge Detectors- "saves" of certain pins to detect edges for interrupts.
DffSynch EDGE_H (
	.D(edgeHD),
	.Q(edgeHQ),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynch EDGE_L (
	.D(edgeLD),
	.Q(edgeLQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Interrupt registers- formal emission of external pin interrupt signals.
DffSynch INT_H (
	.D(intHD),
	.Q(intHQ),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynch INT_L (
	.D(intLD),
	.Q(intLQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Tristate- single driving location for GPIOs (within uP).
Tristate TRI[15:0] (
	.A(triA),
	.Y(triY),
	.S(triEn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute address based controls.
assign isCfgAddr   = ~i_memAddr[1] & ~i_memAddr[0]; // ...00
assign isDirAddr   =  i_memAddr[1] & ~i_memAddr[0]; // ...10
assign isOutAddr   =  i_memAddr[1] &  i_memAddr[0]; // ...11

//------------------------------------------------------------------------------
// Handle config register inputs (and distinct outputs).
assign cfgD    = {i_memDataIn[14], i_memDataIn[12:8]}; // align UART/I2C configs
assign cfgEn   = isCfgAddr & i_memWrEn;
assign cfgUart = cfgQ[5];                              // bit 14 (low UART pin)
assign cfgI2C  = cfgQ[4];                              // bit 12 (low I2C pin)
assign cfgTMR2 = cfgQ[3];                              // bit 11 (TMR2 pin)
assign cfgTMR3 = cfgQ[2];                              // bit 10 (TMR3 pin)
assign cfgIntH = cfgQ[1];                              // bit 9  (High Int pin)
assign cfgIntL = cfgQ[0];                              // bit 8  (Low Int pin)

//------------------------------------------------------------------------------
// Handle synchronizer inputs.
assign inpA = io_gpioPins;

//------------------------------------------------------------------------------
// Handle direction register inputs.
assign dirD  = i_memDataIn;
assign dirEn = isDirAddr & i_memWrEn;

//------------------------------------------------------------------------------
// Handle output register inputs.
assign outD  = i_memDataIn;
assign outEn = isOutAddr & i_memWrEn;

//------------------------------------------------------------------------------
// Handle edge detector inputs.
assign edgeHD = inpY[9]; // bit 9 (High Int pin)
assign edgeLD = inpY[8]; // bit 8 (Low Int pin)

//------------------------------------------------------------------------------
// Handle interrupt inputs.
assign intHD = (inpY[9] ^ edgeHQ) & (cfgIntH ^ edgeHQ);
assign intLD = (inpY[8] ^ edgeLQ) & (cfgIntL ^ edgeLQ);

//------------------------------------------------------------------------------
// Handle tristate inputs.
Mux2 M0(
	.A(i_pwmTmr3),            // Use Alt? use TMR3 PWM signal
	.B(outQ[10]),             // No?      use gpio bit 10
	.S(cfgTMR3),
	.Y(curBit10)
);
Mux2 M1(
	.A(i_pwmTmr2),            // Use Alt? use TMR2 PWM signal
	.B(outQ[11]),             // No?      use gpio bit 11
	.S(cfgTMR2),
	.Y(curBit11)
);
Mux2 M2(
	.A(i_uartTX),             // Use Alt? use UART TX signal
	.B(outQ[15]),             // No?      use gpio bit 15
	.S(cfgUart),
	.Y(curBit15)
);
Mux2 M3(
	.A(i_i2cSDADir),          // Use Alt? use I2C SDA direction
	.B(dirQ[12]),             // No?      use gpio bit 12 direction
	.S(cfgI2C),
	.Y(curDir12)
);
Mux2 M4(
	.A(i_i2cSCLDir),          // Use Alt? use I2C SCL direction
	.B(dirQ[13]),             // No?      use gpio bit 13
	.S(cfgI2C),
	.Y(curDir13)
);

assign triA  = {curBit15,    // UART vs GPIO[15] (TX)
                outQ[14],    // GPIO[14] (UART RX- read only)
					 outQ[13:12], // I2C vs GPIO[13:12] (I2C drive only)
                curBit11,    // TMR2 vs GPIO[11]
					 curBit10,    // TMR3 vs GPIO[10]
					 outQ[9:0]};  // GPIO[9:0]
assign triEn = {dirQ[15:14], // GPIO[15:14]
					 curDir13,    // I2C vs GPIO[12]
					 curDir12,    // I2C vs GPIO[12]
					 dirQ[11:0]};

//------------------------------------------------------------------------------
// Drive data output based on given address.
assign readCfgReg = {1'b0, 
                     cfgUart,      // bit 14
							1'b0, 
							cfgI2C,       // bit 12
							cfgTMR2,      // bit 11
							cfgTMR3,      // bit 10
							cfgIntH,      // bit 9
							cfgIntL,      // bit 8
							8'b00000000};
Mux4 M5[15:0] (
	.C(readCfgReg),                 // Address 00? Read cofigs
	.D(inpY),                       // Address 01? Read pins
	.E(dirQ),                       // Address 10? Read directions
	.F(outQ),                       // Address 11? Read outputs
	.S(i_memAddr),
	.Y(o_memDataOut)
);

//------------------------------------------------------------------------------
// Drive the external pin interrupts.
assign o_intEXH = intHQ;
assign o_intEXL = intLQ;

//------------------------------------------------------------------------------
// Drive the UART's internal read line.
assign o_uartRX = inpY[14] | ~cfgUart; // read "1" when disabled

//------------------------------------------------------------------------------
// Drive the I2C's internal read line.
assign o_i2cSDAOut = inpY[12] | ~cfgI2C; // read "1" when disabled

//------------------------------------------------------------------------------
// Drive gpio pins using tristate.
assign io_gpioPins = triY;

endmodule
