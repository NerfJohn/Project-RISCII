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
	input         i_memRdEn,
	output [15:0] o_memDataOut,
	
	// State machine connections.
	input         i_smIsBooted,
	input         i_smStartPause,
	output        o_smNowPaused,

	// Reported Info connections.
	input  [15:0] i_reportSP,
	input  [14:0] i_reportPC,
	input         i_reportHLT,
	
	// Output Control connections.
	output [3:0]  o_intCode,
	output        o_intEn,
	output        o_doPause,
	
	// Output reset connection.
	output        o_doReset,
	
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
wire        isCctrlAddr, isNvicAddr, isWdtAddr, isGpioAddr;
wire        isTmr0Addr, isTmr1Addr, isTmr2Addr, isTmr3Addr;
wire        isUartAddr, isI2CAddr;

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
wire        nvicIntTM0, nvicIntTM1, nvicIntTM2, nvicIntTM3;
wire        nvicIntUTX, nvicIntURX, nvicIntI2C;
wire [3:0]  nvicIntCode;
wire        nvicIntEn;

// WDT wires.
wire [1:0]  wdtMemAddr;
wire [15:0] wdtMemDataIn, wdtMemDataOut;
wire        wdtMemWrEn;
wire        wdtSmIsBooted, wdtSmStartPause;
wire        wdtDoReset;

// GPIO wires.
wire [1:0]  gpioMemAddr;
wire [15:0] gpioMemDataIn, gpioMemDataOut;
wire        gpioMemWrEn;
wire        gpioIntEXH, gpioIntEXL;
wire        gpioPwmTmr2, gpioPwmTmr3;
wire        gpioUartTX, gpioUartRX;
wire        gpioI2CSCLDir, gpioI2CSDADir, gpioI2CSDAOut;

// Timer 0 wires.
wire [1:0]  tmr0MemAddr;
wire [15:0] tmr0MemDataIn, tmr0MemDataOut;
wire        tmr0MemWrEn;
wire        tmr0SmIsBooted, tmr0SmStartPause;
wire        tmr0IntTMR;

// Timer 1 wires.
wire [1:0]  tmr1MemAddr;
wire [15:0] tmr1MemDataIn, tmr1MemDataOut;
wire        tmr1MemWrEn;
wire        tmr1SmIsBooted, tmr1SmStartPause;
wire        tmr1IntTMR;

// Timer 2 wires.
wire [1:0]  tmr2MemAddr;
wire [15:0] tmr2MemDataIn, tmr2MemDataOut;
wire        tmr2MemWrEn;
wire        tmr2SmIsBooted, tmr2SmStartPause;
wire        tmr2IntTMR, tmr2PwmOut;

// Timer 3 wires.
wire [1:0]  tmr3MemAddr;
wire [15:0] tmr3MemDataIn, tmr3MemDataOut;
wire        tmr3MemWrEn;
wire        tmr3SmIsBooted, tmr3SmStartPause;
wire        tmr3IntTMR, tmr3PwmOut;

// UART wires.
wire [1:0]  uartMemAddr;
wire [15:0] uartMemDataIn, uartMemDataOut;
wire        uartMemWrEn, uartMemRdEn;
wire        uartSmIsBooted, uartSmStartPause, uartSmNowPaused;
wire        uartPinRX, uartPinTX;
wire        uartIntUTX, uartIntURX;

// I2C wires.
wire [1:0]  i2cMemAddr;
wire [15:0] i2cMemDataIn, i2cMemDataOut;
wire        i2cMemWrEn;
wire        i2cSmIsBooted, i2cSmStartPause, i2cSmNowPaused;
wire        i2cPinSDAIn, i2cPinSCLDir, i2cPinSDADir;
wire        i2cIntI2C;

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
	.i_intURX(nvicIntURX),
	.i_intTM0(nvicIntTM0),
	.i_intTM1(nvicIntTM1),
	.i_intTM2(nvicIntTM2),
	.i_intTM3(nvicIntTM3),
	.i_intUTX(nvicIntUTX),
	.i_intI2C(nvicIntI2C),
	.i_intEXL(nvicIntEXL),
	
	// Output interrupt connections.
	.o_intCode(nvicIntCode),
	.o_intEn(nvicIntEn),
	
	// Common signals.
	.i_clk(i_clk),
	.i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Watchdog- special timer that causes a hardware reset when not turned off.
Watchdog WDT (
	// Memory Map connections.
	.i_memAddr(wdtMemAddr),
	.i_memDataIn(wdtMemDataIn),
	.i_memWrEn(wdtMemWrEn),
	.o_memDataOut(wdtMemDataOut),
	
	// State input connections.
	.i_smIsBooted(wdtSmIsBooted),
	.i_smStartPause(wdtSmStartPause),
	
	// Triggered reset connection.
	.o_doReset(wdtDoReset),
	
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
	
	// Alternate pin connections.
	.i_pwmTmr2(gpioPwmTmr2),
	.i_pwmTmr3(gpioPwmTmr3),
	.i_uartTX(gpioUartTX),
	.i_i2cSCLDir(gpioI2CSCLDir),
	.i_i2cSDADir(gpioI2CSDADir),
	.o_uartRX(gpioUartRX),
	.o_i2cSDAOut(gpioI2CSDAOut),
	
	// Raw pinout to outside uP.
	.io_gpioPins(io_gpioPins),     // inout- direct connect net
	
	// Common signals.
	.i_clk(i_clk),
	.i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Timer 0- Basic configurable timer, capable of generating interrupt.
Timer TMR0 (
	// Memory Map connections.
	.i_memAddr(tmr0MemAddr),
   .i_memDataIn(tmr0MemDataIn),
	.i_memWrEn(tmr0MemWrEn),
	.o_memDataOut(tmr0MemDataOut),
	
	// State input connections.
	.i_smIsBooted(tmr0SmIsBooted),
	.i_smStartPause(tmr0SmStartPause),
	
	// Interrupt connection.
	.o_intTMR(tmr0IntTMR),
	
	// Common signals.
	.i_clk(i_clk),
	.i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Timer 1- Basic configurable timer, capable of generating interrupt.
Timer TMR1 (
	// Memory Map connections.
	.i_memAddr(tmr1MemAddr),
   .i_memDataIn(tmr1MemDataIn),
	.i_memWrEn(tmr1MemWrEn),
	.o_memDataOut(tmr1MemDataOut),
	
	// State input connections.
	.i_smIsBooted(tmr1SmIsBooted),
	.i_smStartPause(tmr1SmStartPause),
	
	// Interrupt connection.
	.o_intTMR(tmr1IntTMR),
	
	// Common signals.
	.i_clk(i_clk),
	.i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Timer 2- Configurable timer, capable of creating an interrupt and PWM signal.
TimerPWM TMR2 (
	// Memory Map connections.
	.i_memAddr(tmr2MemAddr),
   .i_memDataIn(tmr2MemDataIn),
	.i_memWrEn(tmr2MemWrEn),
	.o_memDataOut(tmr2MemDataOut),
	
	// State input connections.
	.i_smIsBooted(tmr2SmIsBooted),
	.i_smStartPause(tmr2SmStartPause),
	
	// Interrupt connection.
	.o_intTMR(tmr2IntTMR),
	
	// PWM connection.
	.o_pwmOut(tmr2PwmOut),
	
	// Common signals.
	.i_clk(i_clk),
	.i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Timer 3- Configurable timer, capable of creating an interrupt and PWM signal.
TimerPWM TMR3 (
	// Memory Map connections.
	.i_memAddr(tmr3MemAddr),
   .i_memDataIn(tmr3MemDataIn),
	.i_memWrEn(tmr3MemWrEn),
	.o_memDataOut(tmr3MemDataOut),
	
	// State input connections.
	.i_smIsBooted(tmr3SmIsBooted),
	.i_smStartPause(tmr3SmStartPause),
	
	// Interrupt connection.
	.o_intTMR(tmr3IntTMR),
	
	// PWM connection.
	.o_pwmOut(tmr3PwmOut),
	
	// Common signals.
	.i_clk(i_clk),
	.i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// UART- UART transceiver w/ configurable baud. uP's primary serial port.
Uart UART (
	// Memory Map connections.
	.i_memAddr(uartMemAddr),
	.i_memDataIn(uartMemDataIn),
	.i_memWrEn(uartMemWrEn),
	.i_memRdEn(uartMemRdEn),
	.o_memDataOut(uartMemDataOut),
	
	// State input connections.
	.i_smIsBooted(uartSmIsBooted),
	.i_smStartPause(uartSmStartPause),
	.o_smNowPaused(uartSmNowPaused),
	
	// Serial pin connections.
	.i_pinRX(uartPinRX),
	.o_pinTX(uartPinTX),
	
	// Interrupt connections.
	.o_intUTX(uartIntUTX),
	.o_intURX(uartIntURX),
	
	// Common signals.
	.i_clk(i_clk),
	.i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// I2C- I2C master w/ configurable baud. uP's secondary serial port.
I2C I2C (
	// Memory Map connections.
	.i_memAddr(i2cMemAddr),
	.i_memDataIn(i2cMemDataIn),
	.i_memWrEn(i2cMemWrEn),
	.o_memDataOut(i2cMemDataOut),
	
	// State input connections.
	.i_smIsBooted(i2cSmIsBooted),
	.i_smStartPause(i2cSmStartPause),
	.o_smNowPaused(i2cSmNowPaused),
	
	// Serial pin connections.
	.i_pinSDAIn(i2cPinSDAIn),
	.o_pinSCLDir(i2cPinSCLDir),
	.o_pinSDADir(i2cPinSDADir),
	
	// Interrupt connection.
	.o_intI2C(i2cIntI2C),

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
assign isWdtAddr   = is6BitAddr & ~i_memAddr[5] & ~i_memAddr[4]  // ...0010xx
                                &  i_memAddr[3] & ~i_memAddr[2];
assign isGpioAddr  = is6BitAddr & ~i_memAddr[5] & ~i_memAddr[4]  // ...0011xx
                                &  i_memAddr[3] &  i_memAddr[2];
assign isTmr0Addr  = is6BitAddr & ~i_memAddr[5] &  i_memAddr[4]  // ...0100xx
                                & ~i_memAddr[3] & ~i_memAddr[2];
assign isTmr1Addr  = is6BitAddr & ~i_memAddr[5] &  i_memAddr[4]  // ...0101xx
                                & ~i_memAddr[3] &  i_memAddr[2];
assign isTmr2Addr  = is6BitAddr & ~i_memAddr[5] &  i_memAddr[4]  // ...0110xx
                                &  i_memAddr[3] & ~i_memAddr[2];
assign isTmr3Addr  = is6BitAddr & ~i_memAddr[5] &  i_memAddr[4]  // ...0111xx
                                &  i_memAddr[3] &  i_memAddr[2];
assign isUartAddr  = is6BitAddr &  i_memAddr[5] & ~i_memAddr[4]  // ...1000xx
                                & ~i_memAddr[3] & ~i_memAddr[2];
assign isI2CAddr   = is6BitAddr &  i_memAddr[5] & ~i_memAddr[4]  // ...1001xx
                                & ~i_memAddr[3] &  i_memAddr[2];
										  
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
assign nvicIntURX    = uartIntURX;
assign nvicIntTM0    = tmr0IntTMR;
assign nvicIntTM1    = tmr1IntTMR;
assign nvicIntTM2    = tmr2IntTMR;
assign nvicIntTM3    = tmr3IntTMR;
assign nvicIntUTX    = uartIntUTX;
assign nvicIntI2C    = i2cIntI2C;
assign nvicIntEXL    = gpioIntEXL;

//------------------------------------------------------------------------------
// Handle WDT inputs.
assign wdtMemAddr      = i_memAddr[1:0];
assign wdtMemDataIn    = i_memDataIn;
assign wdtMemWrEn      = isWdtAddr & i_memWrEn;
assign wdtSmIsBooted   = i_smIsBooted;
assign wdtSmStartPause = i_smStartPause;

//------------------------------------------------------------------------------
// Handle GPIO inputs.
assign gpioMemAddr   = i_memAddr[1:0];
assign gpioMemDataIn = i_memDataIn;
assign gpioMemWrEn   = isGpioAddr & i_memWrEn;
assign gpioPwmTmr2   = tmr2PwmOut;
assign gpioPwmTmr3   = tmr3PwmOut;
assign gpioUartTX    = uartPinTX;
assign gpioI2CSCLDir = i2cPinSCLDir;
assign gpioI2CSDADir = i2cPinSDADir;

//------------------------------------------------------------------------------
// Handle Timer 0 inputs.
assign tmr0MemAddr      = i_memAddr[1:0];
assign tmr0MemDataIn    = i_memDataIn;
assign tmr0MemWrEn      = isTmr0Addr & i_memWrEn;
assign tmr0SmIsBooted   = i_smIsBooted;
assign tmr0SmStartPause = i_smStartPause;

//------------------------------------------------------------------------------
// Handle Timer 1 inputs.
assign tmr1MemAddr      = i_memAddr[1:0];
assign tmr1MemDataIn    = i_memDataIn;
assign tmr1MemWrEn      = isTmr1Addr & i_memWrEn;
assign tmr1SmIsBooted   = i_smIsBooted;
assign tmr1SmStartPause = i_smStartPause;

//------------------------------------------------------------------------------
// Handle Timer 2 inputs.
assign tmr2MemAddr      = i_memAddr[1:0];
assign tmr2MemDataIn    = i_memDataIn;
assign tmr2MemWrEn      = isTmr2Addr & i_memWrEn;
assign tmr2SmIsBooted   = i_smIsBooted;
assign tmr2SmStartPause = i_smStartPause;

//------------------------------------------------------------------------------
// Handle Timer 3 inputs.
assign tmr3MemAddr      = i_memAddr[1:0];
assign tmr3MemDataIn    = i_memDataIn;
assign tmr3MemWrEn      = isTmr3Addr & i_memWrEn;
assign tmr3SmIsBooted   = i_smIsBooted;
assign tmr3SmStartPause = i_smStartPause;

//------------------------------------------------------------------------------
// Handle UART inputs.
assign uartMemAddr      = i_memAddr[1:0];
assign uartMemDataIn    = i_memDataIn;
assign uartMemWrEn      = isUartAddr & i_memWrEn;
assign uartMemRdEn      = isUartAddr & i_memRdEn;
assign uartSmIsBooted   = i_smIsBooted;
assign uartSmStartPause = i_smStartPause;
assign uartPinRX        = gpioUartRX;

//------------------------------------------------------------------------------
// Handle I2C inputs.
assign i2cMemAddr      = i_memAddr[1:0];
assign i2cMemDataIn    = i_memDataIn;
assign i2cMemWrEn      = isI2CAddr & i_memWrEn;
assign i2cSmIsBooted   = i_smIsBooted;
assign i2cSmStartPause = i_smStartPause;
assign i2cPinSDAIn     = gpioI2CSDAOut;

//------------------------------------------------------------------------------
// Drive data output based on given address.
Mux4 M0[15:0] (
	.C(cctrlMemDataOut),      // Addr 0000-xx? Read CCTRL
	.D(nvicMemDataOut),       // Addr 0001-xx? Read NVIC
	.E(wdtMemDataOut),        // Addr 0010-xx? Read WDT
	.F(gpioMemDataOut),       // Addr 0011-xx? Read GPIO
	.S(i_memAddr[3:2]),
	.Y(readData00XX)
);
Mux4 M1[15:0] (
	.C(tmr0MemDataOut),       // Addr 0100-xx? Read TMR0
	.D(tmr1MemDataOut),       // Addr 0101-xx? Read TMR1
	.E(tmr2MemDataOut),       // Addr 0110-xx? Read TMR2
	.F(tmr3MemDataOut),       // Addr 0111-xx? Read TMR3
	.S(i_memAddr[3:2]),
	.Y(readData01XX)
);
Mux4 M2[15:0] (
	.C(uartMemDataOut),       // Addr 1000-xx? Read UART
	.D(i2cMemDataOut),        // Addr 1001-xx? Read I2C
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
// Drive state machine outputs.
assign o_smNowPaused = uartSmNowPaused & i2cSmNowPaused;

//------------------------------------------------------------------------------
// Drive control outputs.
assign o_intCode = nvicIntCode;
assign o_intEn   = nvicIntEn;
assign o_doPause = cctrlDoPause;

//------------------------------------------------------------------------------
// Drive reset outputs.
assign o_doReset = wdtDoReset;

endmodule
