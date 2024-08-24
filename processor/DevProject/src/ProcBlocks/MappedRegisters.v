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
wire        isCctrlAddr;

// Core Control wires.
wire [1:0]  cctrlMemAddr;
wire [15:0] cctrlMemDataIn, cctrlMemDataOut;
wire        cctrlMemWrEn;

// Compute data wires (based on mem address).
wire [15:0] readAddr0X;

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
	
	// Common signals.
	.i_clk(i_clk),
	.i_rstn(i_rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute mem address based controls.
assign is6BitAddr = ~(|i_memAddr[13:6]);
assign isCctrlAddr = is6BitAddr & ~i_memAddr[5] & ~i_memAddr[4]  // ...0000xx
                                & ~i_memAddr[3] & ~i_memAddr[2];
										  
//------------------------------------------------------------------------------
// Handle Core Control (cctrl) inputs.
assign cctrlMemAddr   = i_memAddr[1:0];
assign cctrlMemDataIn = i_memDataIn;
assign cctrlMemWrEn   = isCctrlAddr & i_memWrEn;

//------------------------------------------------------------------------------
// Drive data output based on given address.
assign o_memDataOut = cctrlMemDataOut & {16{isCctrlAddr}};

endmodule
