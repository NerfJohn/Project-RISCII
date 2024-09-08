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
	
	// Raw pinout to outside uP.
	inout  [15:0] io_gpioPins,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * TODO- desc once all funcs are made.
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

// Tristate wires.
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
// Handle tristate inputs.
assign triA  = outQ; // TODO- implement
assign triEn = dirQ; // TODO- implement (individual tristates).

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
Mux4 M0[15:0] (
	.C(readCfgReg),                 // Address 00? Read controls
	.D(inpY),                       // Address 01? Read setpoint
	.E(dirQ),                       // Address 10? Read PC
	.F(outQ),                       // Address 11? Read SP
	.S(i_memAddr),
	.Y(o_memDataOut)
);

//------------------------------------------------------------------------------
// Drive gpio pins using tristate.
assign io_gpioPins = triY;

endmodule
