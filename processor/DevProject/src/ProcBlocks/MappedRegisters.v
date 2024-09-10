/*
 * MappedRegisters.v
 *
 * "Collection of mapped register/peripherals on memory bus"
 */
module MappedRegisters (
	// Memory Bus connections.
	input  [13:0] i_memAddr,
	input  [15:0] i_memDataIn,
	input         i_memWrEn,
	output [15:0] o_memDataOut,

	// Reported Info connections.
	input  [15:0] i_reportSP,
	input  [14:0] i_reportPC,
	input         i_reportHLT,
	
	// Output Control connections.
	output [3:0]  o_intCode,
	output        o_intEn,
	output        o_doPause,
	
	// Driven GPIO connections.
	inout  [15:0] io_gpioPins,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * TODO- desc once all funcs are done.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Compute control wires (based on mem address).
wire        is6BitAddr;
wire        isCctrlAddr, isNvicAddr, isGpioAddr;

// Core Control wires.
wire [1:0]  cctrlMemAddr;
wire [15:0] cctrlMemDataIn, cctrlMemDataOut;
wire        cctrlMemWrEn;
wire [15:0] cctrlReportSP;
wire [14:0] cctrlReportPC;
wire        cctrlReportHLT;
wire        cctrlDoPause, cctrlIntOVF;

// NVIC wires.
wire [1:0]  nvicMemAddr;
wire [15:0] nvicMemDataIn, nvicMemDataOut;
wire        nvicMemWrEn;
wire        nvicIntOVF, nvicIntEXH, nvicIntEXL;
wire [3:0]  nvicIntCode;
wire        nvicIntEn;

// GPIO wires.
wire [1:0]  gpioMemAddr;
wire [15:0] gpioMemDataIn, gpioMemDataOut;
wire        gpioMemWrEn;
wire        gpioIntEXH, gpioIntEXL;

// Compute data wires (based on mem address).
wire [15:0] readData00XX, readData01XX, readData10XX;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Core Control- stack overflow detection and pause source managed by core.
CoreCtrl CORE_CTRL (
	// Memory Map connections.
	.i_memAddr(cctrlMemAddr),
	.i_memDataIn(cctrlMemDataIn),
	.i_memWrEn(cctrlMemWrEn),
	.o_memDataOut(cctrlMemDataOut),
	
	// Reported Info connections.
	.i_reportSP(cctrlReportSP),
	.i_reportPC(cctrlReportPC),
	.i_reportHLT(cctrlReportHLT),
	
	// Output Control connections.
	.o_doPause(cctrlDoPause),
	.o_intOVF(cctrlIntOVF),
	
	// Common signals.
	.i_clk(i_clk),
	.i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// NVIC- interrupt controller to send core processing interrupt signals.
Nvic NVIC (
	// Memory Map connections.
	.i_memAddr(nvicMemAddr),
	.i_memDataIn(nvicMemDataIn),
	.i_memWrEn(nvicMemWrEn),
	.o_memDataOut(nvicMemDataOut),
	
	// Input flag connections.
	.i_intOVF(nvicIntOVF),
	.i_intEXH(nvicIntEXH),
	.i_intEXL(nvicIntEXL),
	
	// Output interrupt connections.
	.o_intCode(nvicIntCode),
	.o_intEn(nvicIntEn),
	
	// Common signals.
	.i_clk(i_clk),
	.i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// GPIO- Generic IO pins, some with alternate functions and interrupt settings.
Gpio GPIO (
	// Memory Map connections.
	.i_memAddr(gpioMemAddr),
	.i_memDataIn(gpioMemDataIn),
	.i_memWrEn(gpioMemWrEn),
	.o_memDataOut(gpioMemDataOut),
	
	// Interrupt signal connections.
	.o_intEXH(gpioIntEXH),
	.o_intEXL(gpioIntEXL),
	
	// Raw pinout to outside uP.
	.io_gpioPins(io_gpioPins),     // inout- direct connect net
	
	// Common signals.
	.i_clk(i_clk),
	.i_rstn(i_rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute mem address based controls.
assign is6BitAddr  = ~(|i_memAddr[13:6]);
assign isCctrlAddr = is6BitAddr & ~i_memAddr[5] & ~i_memAddr[4]  // ...0000xx
                                & ~i_memAddr[3] & ~i_memAddr[2];
assign isNvicAddr  = is6BitAddr & ~i_memAddr[5] & ~i_memAddr[4]  // ...0001xx
                                & ~i_memAddr[3] &  i_memAddr[2];
assign isGpioAddr  = is6BitAddr & ~i_memAddr[5] & ~i_memAddr[4]  // ...0011xx
                                &  i_memAddr[3] &  i_memAddr[2];
										  
//------------------------------------------------------------------------------
// Handle Core Control (cctrl) inputs.
assign cctrlMemAddr   = i_memAddr[1:0];
assign cctrlMemDataIn = i_memDataIn;
assign cctrlMemWrEn   = isCctrlAddr & i_memWrEn;
assign cctrlReportSP  = i_reportSP;
assign cctrlReportPC  = i_reportPC;
assign cctrlReportHLT = i_reportHLT;

//------------------------------------------------------------------------------
// Handle NVIC inputs.
assign nvicMemAddr   = i_memAddr[1:0];
assign nvicMemDataIn = i_memDataIn;
assign nvicMemWrEn   = isNvicAddr & i_memWrEn;
assign nvicIntOVF    = cctrlIntOVF;
assign nvicIntEXH    = gpioIntEXH;
assign nvicIntEXL    = gpioIntEXL;

//------------------------------------------------------------------------------
// Handle GPIO inputs.
assign gpioMemAddr   = i_memAddr[1:0];
assign gpioMemDataIn = i_memDataIn;
assign gpioMemWrEn   = isGpioAddr & i_memWrEn;

//------------------------------------------------------------------------------
// Drive data output based on given address.
Mux4 M0[15:0] (
	.C(cctrlMemDataOut),      // Addr 0000-xx? Read CCTRL
	.D(nvicMemDataOut),       // Addr 0001-xx? Read NVIC
	.E(16'b0000000000000000), // Addr 0010-xx? TODO- implement
	.F(gpioMemDataOut),       // Addr 0011-xx? Read GPIO
	.S(i_memAddr[3:2]),
	.Y(readData00XX)
);
Mux4 M1[15:0] (
	.C(16'b0000000000000000), // Addr 0100-xx? TODO- implement
	.D(16'b0000000000000000), // Addr 0101-xx? TODO- implement
	.E(16'b0000000000000000), // Addr 0110-xx? TODO- implement
	.F(16'b0000000000000000), // Addr 0111-xx? TODO- implement
	.S(i_memAddr[3:2]),
	.Y(readData01XX)
);
Mux4 M2[15:0] (
	.C(16'b0000000000000000), // Addr 1000-xx? TODO- implement
	.D(16'b0000000000000000), // Addr 1001-xx? TODO- implement
	.E(16'b0000000000000000), // Addr 1010-xx? TODO- implement
	.F(16'b0000000000000000), // Addr 1011-xx? No mapped registers
	.S(i_memAddr[3:2]),
	.Y(readData10XX)
);
Mux4 M3[15:0] (
	.C(readData00XX),         // Addr 00xx-xx? Process addrs 0000-xx to 0011-xx
	.D(readData01XX),         // Addr 01xx-xx? Process addrs 0100-xx to 0111-xx
	.E(readData10XX),         // Addr 10xx-xx? Process addrs 1000-xx to 1011-xx
	.F(16'b0000000000000000), // Addr 11xx-xx? No mapped registers
	.S(i_memAddr[5:4]),
	.Y(o_memDataOut)
);

//------------------------------------------------------------------------------
// Drive control outputs.
assign o_intCode = nvicIntCode;
assign o_intEn   = nvicIntEn;
assign o_doPause = cctrlDoPause;

endmodule
