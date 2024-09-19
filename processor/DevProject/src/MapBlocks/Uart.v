/*
 * Uart.v
 *
 * "Mapped UART with full duplex communication at set baud rate"
 */
module Uart (
	// Memory Map connections.
	input  [1:0]  i_memAddr,
	input  [15:0] i_memDataIn,
	input         i_memWrEn,
	output [15:0] o_memDataOut,
	
	// State input connections.
	input         i_smIsBooted,
	input         i_smStartPause,
	output        o_smNowPaused,
	
	// Serial pin connections.
	output        o_pinTX,
	
	// Interrupt connections.
	output        o_intUTX,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * TODO- write once done w/ file
 */
 
////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Compute control wires (based on mem access/uP state).
wire        isCtrlAddr, isBaudAddr, isTxAddr;
wire        isBaudWr, isTxWr;
wire        doPause;

// Control setting wires.
wire        enableD, enableQ, enableEn;

// Baud rate wires.
wire [14:0] baudD, baudQ;
wire        baudEn;

// TX wires.
wire [7:0]  txD, txQ;
wire        txEn;

// Compute control wires (based off TX state).
wire        txIsIdle, txIsFront, txIsByte, txIsEnd;
wire        txIsBaudMatch; 
wire        txDoStart;

// TX state wires.
wire [15:0] txBaudQ;
wire        txBaudRstn;
wire [3:0]  txAddA, txAddB, txAddS;
wire        txAddI;
wire [3:0]  txStateD, txStateQ;
wire        txStateEn, txStateRstn;

// interrupt wires.
wire        intTXD, intTXQ;

// pause wires.
wire        pauseD, pauseQ;

// Mem read wires.
wire [15:0] readCtrlReg;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Control settings- general controls used to direct UART behavior.
DffSynchEn ENABLE (
	.D(enableD),
	.Q(enableQ),
	.S(enableEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Baud rate- transmission rate (in clk ticks per UART tick) of TX/RX bytes.
DffSynchEn BAUD_RATE[14:0] (
	.D(baudD),
	.Q(baudQ),
	.S(baudEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// TX byte- data byte to transmit over the TX pin.
DffSynchEn TX_BYTE[7:0] (
	.D(txD),
	.Q(txQ),
	.S(txEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// TX State- state machine used to control TX process.
RegInc16 TX_BAUD (
	.Q(txBaudQ),
	.clk(i_clk),
	.rstn(txBaudRstn)
);
Add4 TX_ADD (
	.A(txAddA),
	.B(txAddB),
	.I(txAddI),
	.S(txAddS)
);
DffSynchEn TX_STATE[3:0] (
	.D(txStateD),
	.Q(txStateQ),
	.S(txStateEn),
	.clk(i_clk),
	.rstn(txStateRstn)
);

//------------------------------------------------------------------------------
// Interrupts- pulsed per channel upon one byte of transfer.
DffSynch INT_UTX (
	.D(intTXD),
	.Q(intTXQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Pause register- UART formally/locally pauses once both channels are idle.
DffSynch PAUSE (
	.D(pauseD),
	.Q(pauseQ),
	.clk(i_clk),
	.rstn(i_rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute access/state based controls.
assign isCtrlAddr = ~i_memAddr[1] & ~i_memAddr[0]; // ...00
assign isBaudAddr = ~i_memAddr[1] &  i_memAddr[0]; // ...01
assign isTxAddr   =  i_memAddr[1] & ~i_memAddr[0]; // ...10
assign isBaudWr   = isBaudAddr & i_memWrEn;
assign isTxWr     = isTxAddr   & i_memWrEn;
assign doPause    = ~i_smIsBooted | i_smStartPause | pauseQ;

//------------------------------------------------------------------------------
// Handle control inputs.
assign enableD  = i_memDataIn[0];
assign enableEn = isCtrlAddr & i_memWrEn;

//------------------------------------------------------------------------------
// Handle baud inputs.
assign baudD  = i_memDataIn[15:1];
assign baudEn = isBaudWr;

//------------------------------------------------------------------------------
// Handle TX inputs.
Mux2 M0[7:0] (
	.A(i_memDataIn[7:0]), // Writing TX? Use write data
	.B({1'b0, txQ[7:1]}), // No?         Use shift right logical
	.S(isTxWr),
	.Y(txD)
);
assign txEn = isTxWr | (txIsByte & txIsBaudMatch);

//------------------------------------------------------------------------------
// Compute tx state based controls.
assign txIsIdle      = ~(|txStateQ);
assign txIsFront     = ~txStateQ[3] & ~txStateQ[2] & ~txStateQ[1] & txStateQ[0];
assign txIsByte      = txStateQ[3] ^ (txStateQ[2] | txStateQ[1]);
assign txIsEnd       = txStateQ[3] & ~txStateQ[2] & txStateQ[1] & ~txStateQ[0];
assign txIsBaudMatch = ~(|(txBaudQ[15:1] ^ baudQ));
assign txDoStart     = enableQ & txIsIdle & isTxWr & ~doPause;

//------------------------------------------------------------------------------
// Handle TX state inputs.
assign txBaudRstn   = i_rstn & ~txIsIdle & ~txIsBaudMatch;
assign txAddA       = txStateQ;
assign txAddB       = 4'b0000;                                   // using as inc
assign txAddI       = 1'b1;                                      // using as inc
assign txStateD     = txAddS & {4{~txIsEnd}};
assign txStateEn    = txDoStart | (~txIsIdle & txIsBaudMatch);
assign txStateRstn  = i_rstn;

//------------------------------------------------------------------------------
// Handle interrupt inputs.
assign intTXD = txIsEnd & txIsBaudMatch & enableQ & ~doPause;

//------------------------------------------------------------------------------
// Handle pause inputs.
assign pauseD = i_smStartPause & txIsIdle;

//------------------------------------------------------------------------------
// Drive data output based on given address.
assign readCtrlReg = {txIsIdle,           // TX status (is is idle)
	                   14'b00000000000000, // TODO- implement
							 enableQ};           // Enable control
Mux4 M1[15:0] (
	.C(readCtrlReg),                       // Address 00? Read controls/statuses // TODO- implement
	.D({baudQ, 1'b0}),                     // Address 01? Read baud rate
	.E({8'b00000000, txQ}),                // Address 10? Read TX byte
	.F(16'b0),                             // Address 11? TODO- implement
	.S(i_memAddr),
	.Y(o_memDataOut)
);

//------------------------------------------------------------------------------
// Drive pause status.
assign o_smNowPaused = pauseQ;

//------------------------------------------------------------------------------
// Drive UART TX pin.
assign o_pinTX = ~txIsFront & (txQ[0] | ~txIsByte);

//------------------------------------------------------------------------------
// Dribe interrupts.
assign o_intUTX = intTXQ;

endmodule
