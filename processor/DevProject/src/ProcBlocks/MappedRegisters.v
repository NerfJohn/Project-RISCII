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
wire        is6BitAddr, isMapAddr;
wire        isCctrlAddr, isNvicAddr;

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
wire        nvicIntOVF;
wire [3:0]  nvicIntCode;
wire        nvicIntEn;

// Compute data wires (based on mem address).
wire [15:0] readData000X;

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
	
	// Output interrupt connections.
	.o_intCode(nvicIntCode),
	.o_intEn(nvicIntEn),
	
	// Common signals.
	.i_clk(i_clk),
	.i_rstn(i_rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute mem address based controls.
assign isMapAddr   = ~(|i_memAddr[13:3]);                        // ...000xxx
assign is6BitAddr  = ~(|i_memAddr[13:6]);
assign isCctrlAddr = is6BitAddr & ~i_memAddr[5] & ~i_memAddr[4]  // ...0000xx
                                & ~i_memAddr[3] & ~i_memAddr[2];
assign isNvicAddr  = is6BitAddr & ~i_memAddr[5] & ~i_memAddr[4]  // ...0001xx
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

//------------------------------------------------------------------------------
// Drive data output based on given address.
Mux2 M0[15:0] (
	.A(nvicMemDataOut),  // Addr 0001xx? Read NVIC
	.B(cctrlMemDataOut), // Addr 0000xx? Read CCTRL
	.S(i_memAddr[2]),
	.Y(readData000X)
);
assign o_memDataOut = readData000X & {16{isMapAddr}};

//------------------------------------------------------------------------------
// Drive control outputs.
assign o_intCode = nvicIntCode;
assign o_intEn   = nvicIntEn;
assign o_doPause = cctrlDoPause;

endmodule
