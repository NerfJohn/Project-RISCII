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
	input         i_memRdEn,
	output [15:0] o_memDataOut,
	
	// State input connections.
	input         i_smIsBooted,
	input         i_smStartPause,
	output        o_smNowPaused,
	
	// Serial pin connections.
	input         i_pinRX,
	output        o_pinTX,
	
	// Interrupt connections.
	output        o_intUTX,
	output        o_intURX,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * UART transceiver with configurable baud rate. Implcitly "8N1" setup.
 *
 * TX and RX channels share a single baud rate. TX channel allows for directly
 * writing the shift register while RX register is a shadow updated for each
 * sent byte. RX status bit is set on update and cleared on RX read from core.
 */
 
////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Compute control wires (based on mem access/uP state).
wire        isCtrlAddr, isBaudAddr, isTxAddr, isRxAddr;
wire        isBaudWr, isTxWr, isRxRd;
wire        doPause;

// Control setting wires.
wire        enableD, enableQ, enableEn;
wire        rxRdyD, rxRdyQ, rxRdyEn;

// Baud rate wires.
wire [14:0] baudD, baudQ;
wire        baudEn;

// TX wires.
wire [7:0]  txD, txQ;
wire        txEn;

// RX wires.
wire [7:0]  rxD, rxQ;
wire        rxEn;

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
wire        txStateEn;

// Compute control wires (based off RX state).
wire        rxIsIdle, rxIsByte, rxIsEnd;
wire        rxIsBaudMatch; 
wire        rxDoStart, rxDoSample, rxDoUpdate;

// RX state wires.
wire [7:0]  rxSaveD, rxSaveQ;
wire        rxSaveEn;
wire [15:0] rxBaudQ;
wire        rxBaudRstn;
wire [3:0]  rxAddA, rxAddB, rxAddS;
wire        rxAddI;
wire [3:0]  rxStateD, rxStateQ;
wire        rxStateEn;

// interrupt wires.
wire        intTXD, intTXQ;
wire        intRXD, intRXQ;

// pause wires.
wire        pauseD, pauseQ;

// Mem read wires.
wire [15:0] readCtrlReg;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Control settings- general controls/statuses used to direct UART behavior.
DffSynchEn ENABLE (
	.D(enableD),
	.Q(enableQ),
	.S(enableEn),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn RX_RDY (
	.D(rxRdyD),
	.Q(rxRdyQ),
	.S(rxRdyEn),
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
// RX byte- (last full) data byte transmitted over RX pin.
DffSynchEn RX_BYTE[7:0] (
	.D(rxD),
	.Q(rxQ),
	.S(rxEn),
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
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// RX State- state machine used to control RX process.
DffSynchEn RX_SAVE[7:0] (
	.D(rxSaveD),
	.Q(rxSaveQ),
	.S(rxSaveEn),
	.clk(i_clk),
	.rstn(i_rstn)
);
RegInc16 RX_BAUD (
	.Q(rxBaudQ),
	.clk(i_clk),
	.rstn(rxBaudRstn)
);
Add4 RX_ADD (
	.A(rxAddA),
	.B(rxAddB),
	.I(rxAddI),
	.S(rxAddS)
);
DffSynchEn RX_STATE[3:0] (
	.D(rxStateD),
	.Q(rxStateQ),
	.S(rxStateEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Interrupts- pulsed per channel upon one byte of transfer.
DffSynch INT_UTX (
	.D(intTXD),
	.Q(intTXQ),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynch INT_URX (
	.D(intRXD),
	.Q(intRXQ),
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
assign isRxAddr   =  i_memAddr[1] &  i_memAddr[0]; // ...11
assign isBaudWr   = isBaudAddr &  i_memWrEn;
assign isTxWr     = isTxAddr   &  i_memWrEn;
assign isRxRd     = isRxAddr   &  i_memRdEn;
assign doPause    = ~i_smIsBooted | i_smStartPause | pauseQ;

//------------------------------------------------------------------------------
// Handle control inputs.
assign enableD  = i_memDataIn[0];
assign enableEn = isCtrlAddr & i_memWrEn;
assign rxRdyD   = rxDoUpdate | ~isRxRd;             // read timing beats update
assign rxRdyEn  = rxDoUpdate | (isRxRd & ~doPause); // read must be from core

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
// Handle RX inputs.
assign rxD  = rxSaveQ;
assign rxEn = rxDoUpdate;

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

//------------------------------------------------------------------------------
// Compute rx state based controls.
assign rxIsIdle      = ~(|rxStateQ);
assign rxIsByte      = rxStateQ[3] ^ (rxStateQ[2] | rxStateQ[1]);
assign rxIsEnd       = rxStateQ[3] & ~rxStateQ[2] & rxStateQ[1] & ~rxStateQ[0];
assign rxIsBaudMatch = ~(|(rxBaudQ[15:1] ^ baudQ));
assign rxDoStart     = enableQ & rxIsIdle & ~i_pinRX & ~doPause;
assign rxDoSample    = ~(|(rxBaudQ[14:0] ^ baudQ));
assign rxDoUpdate    = rxIsEnd & rxIsBaudMatch & enableQ & ~doPause;

//------------------------------------------------------------------------------
// Handle RX state inputs.
assign rxSaveD      = {i_pinRX, rxSaveQ[7:1]};
assign rxSaveEn     = rxIsByte & rxDoSample;
assign rxBaudRstn   = i_rstn & ~rxIsIdle & ~rxIsBaudMatch;
assign rxAddA       = rxStateQ;
assign rxAddB       = 4'b0000;                                   // using as inc
assign rxAddI       = 1'b1;                                      // using as inc
assign rxStateD     = rxAddS & {4{~rxIsEnd}};
assign rxStateEn    = rxDoStart | (~rxIsIdle & rxIsBaudMatch);

//------------------------------------------------------------------------------
// Handle interrupt inputs.
assign intTXD = txIsEnd & txIsBaudMatch & enableQ & ~doPause;
assign intRXD = rxDoUpdate;

//------------------------------------------------------------------------------
// Handle pause inputs.
assign pauseD = i_smStartPause & txIsIdle;

//------------------------------------------------------------------------------
// Drive data output based on given address.
assign readCtrlReg = {txIsIdle,           // TX status (is is idle)
                      rxRdyQ,             // RX status (has read ready)
	                   13'b0000000000000,
							 enableQ};           // Enable control
Mux4 M1[15:0] (
	.C(readCtrlReg),                       // Address 00? Read controls/statuses
	.D({baudQ, 1'b0}),                     // Address 01? Read baud rate
	.E({8'b00000000, txQ}),                // Address 10? Read TX byte
	.F({8'b00000000, rxQ}),                // Address 11? Read RX byte
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
assign o_intURX = intRXQ;

endmodule
