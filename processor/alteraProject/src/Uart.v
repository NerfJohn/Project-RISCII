module Uart (busAddr, busData, busEn, busWr,  	// Bus Signals
             sigTxInt, sigRxInt, txOut, rxIn,   // Special Signals
				 clk, rstn,                         // Common Signals
				 T_a0, T_a1, T_a2, T_a3					// TODO- test signals, remove
);

//////////////////////////
// -- Inputs/Outputs -- //
//////////////////////////

// Bus Signals.
input 		busEn;		// Enable-  signals r/w of timer register
input 		busWr;		// Write-   specifies bus write (low = bus read)
input[1:0] 	busAddr;		// Address- register to access
inout[15:0] busData;		// Data-    passes read/write data

// Special Signals.
output sigTxInt, sigRxInt;			// Interrupt signals (for TX/RX)
output txOut;                    // TX signal input (from pin)
input rxIn;                      // RX signal output (to pin)

// Common Signals.
input clk, rstn;			// Common clock and active-low reset

output[15:0] T_a0, T_a1, T_a2, T_a3; // TODO- test signals, remove

/////////////////////////
// -- Signals/Wires -- //
/////////////////////////

// Bus-Accessible Registers.
wire[15:0] brgD, brgQ;
wire       ctlD, ctlQ;
wire[9:0]  txrD, txrQ;
wire[9:0]  rxrD, rxrQ;
wire brgEn, ctlEn, txrEn, rxrEn;
wire[15:0] ctlQ_SW, txrQ_SW, rxrQ_SW;	// SW formatting for reads
wire[9:0] txrD_HW;							// HW formatting for writes

// State Machine (TX).
wire txStateD, txStateQ;
wire txBeginXmit;

// State Machine (RX).
wire rxStateD, rxStateQ;
wire rxBeginReceive;

// Baud rate tracker (TX) wires.
wire[15:0] txBaudD, txBaudQ;
wire[15:0] txBaudIncA, txBaudIncY;
wire txBaudMatch;

// Baud rate tracker (RX) wires.
wire[15:0] rxBaudD, rxBaudQ;
wire[15:0] rxBaudIncA, rxBaudIncY;
wire rxBaudMatch;
wire rxTakeSample;

// Bits sent tracker (TX) wires.
wire[3:0] txBitsD, txBitsQ;
wire[3:0] txBitsIncA, txBitsIncY;
wire txBitsEn;
wire txLastBit;

// Bits received tracker (RX) wires.
wire[3:0] rxBitsD, rxBitsQ;
wire[3:0] rxBitsIncA, rxBitsIncY;
wire rxBitsEn;
wire rxLastBit;

// Output/Status bits (TX).
wire txOutD, txOutQ;
wire txDoneD, txDoneQ;

// Output/Statis bits (RX).
wire rxDoneD, rxDoneQ;
wire rxNewD, rxNewQ;

// Tristate Wires (for data line during read).
wire[15:0] triA, triY;
wire       triEn;

// Additional Bus-related wires.
wire regWr;
wire brgAddr, ctlAddr, txrAddr, rxrAddr;
wire[15:0] brgCtl, txrRxr; // Mid-mux value (for tristate input).

////////////////////////////
// -- Blocks/Instances -- //
////////////////////////////

// Bus-Accessible Registers.
dff_en BRG[15:0] (.D(brgD), .Q(brgQ), .en(brgEn), .clk(clk), .rstn(rstn));
dff_en CTL       (.D(ctlD), .Q(ctlQ), .en(ctlEn), .clk(clk), .rstn(rstn));
dff_en TXR[9:0]  (.D(txrD), .Q(txrQ), .en(txrEn), .clk(clk), .rstn(rstn));
dff_en RXR[9:0]  (.D(rxrD), .Q(rxrQ), .en(rxrEn), .clk(clk), .rstn(rstn));

// State machine (TX).
myDff TX_STATE (.D(txStateD), .Q(txStateQ), .clk(clk), .rstn(rstn));

// State machine (RX).
myDff RX_STATE (.D(rxStateD), .Q(rxStateQ), .clk(clk), .rstn(rstn));

// Baud rate tracker (TX).
myDff TX_BAUD[15:0] (.D(txBaudD), .Q(txBaudQ), .clk(clk), .rstn(rstn));
add_16b TX_BAUD_INC (.src1(txBaudIncA), .src2(1'b0), .cin(1'b1), .sum(txBaudIncY), .cout(/*NC*/));

// Baud rate tracker (RX).
myDff RX_BAUD[15:0] (.D(rxBaudD), .Q(rxBaudQ), .clk(clk), .rstn(rstn));
add_16b RX_BAUD_INC (.src1(rxBaudIncA), .src2(1'b0), .cin(1'b1), .sum(rxBaudIncY), .cout(/*NC*/));

// Bits sent tracker (TX).
dff_en TX_BITS[3:0] (.D(txBitsD), .Q(txBitsQ), .en(txBitsEn), .clk(clk), .rstn(rstn));
add_4b TX_BITS_INC (.A(txBitsIncA), .B(1'b0), .Cin(1'b1), .S(txBitsIncY), .Cout(/*NC*/));

// Bits received tracker (RX).
dff_en RX_BITS[3:0] (.D(rxBitsD), .Q(rxBitsQ), .en(rxBitsEn), .clk(clk), .rstn(rstn));
add_4b RX_BITS_INC  (.A(rxBitsIncA), .B(1'b0), .Cin(1'b1), .S(rxBitsIncY), .Cout(/*NC*/));

// Output/Status bits (TX).
myDff TX_INTR (.D(txDoneD), .Q(txDoneQ), .clk(clk), .rstn(rstn));
myDff TX_OUT  (.D(txOutD), .Q(txOutQ), .clk(clk), .rstn(rstn));

// Output/Status bits (RX).
myDff RX_INTR (.D(rxDoneD), .Q(rxDoneQ), .clk(clk), .rstn(rstn));
myDff RX_NEW (.D(rxNewD), .Q(rxNewQ), .clk(clk), .rstn(rstn));

// Tristate.
Tristate TRI[15:0] (.A(triA), .en(triEn), .Y(triY));

//////////////////////////
// -- Connects/Logic -- //
//////////////////////////

// Additonal bus-related wires/signals.
assign regWr = busEn & busWr;
assign brgAddr = ~busAddr[1] & ~busAddr[0]; // addr 0x0
assign ctlAddr = ~busAddr[1] & busAddr[0];  // addr 0x1
assign txrAddr = busAddr[1] & ~busAddr[0];  // addr 0x2
assign rxrAddr = busAddr[1] & busAddr[0];   // addr 0x3

// Bus Registers- mostly r/w access with different HW/SW views.
mux2 iMUX0[9:0] (.A({1'b1, busData[7:0], 1'b0}), 
						.B({1'b0, txrQ[9:1]}), 
						.sel(regWr & txrAddr), 
						.out(txrD_HW)
);
assign ctlQ_SW = {10'b0, rxNewQ, txStateQ, 3'b0, ctlQ}; // to 16-bit "SW" view of enable
assign txrQ_SW = {8'b0, txrQ[8:1]};             // to 16-bit "SW" view of send byte
assign rxrQ_SW = {8'b0, rxrQ[8:1]};             // to 16-bit "SW" view of read byte

assign brgD = busData;
assign ctlD = busData[0];
assign txrD = txrD_HW;
assign rxrD = {rxIn, rxrQ[9:1]};

assign brgEn = regWr & brgAddr;
assign ctlEn = regWr & ctlAddr;
assign txrEn = txBaudMatch | (regWr & txrAddr);
assign rxrEn = rxTakeSample & rxStateQ;

// State Machine (TX).
assign txBeginXmit = ~txStateQ & ctlQ & (regWr & txrAddr);
assign txStateD = txBeginXmit | (txStateQ & ~(txLastBit & txBaudMatch));

// State machine (RX).
assign rxBeginReceive = ~rxStateQ & ~rxIn;
assign rxStateD = rxBeginReceive | (rxStateQ & ~(rxLastBit & rxBaudMatch));

// Baud rate tracking (TX).
assign txBaudMatch = ~(|(txBaudQ ^ brgQ));
assign txBaudIncA = txBaudQ;
assign txBaudD = txBaudIncY & ~({16{txBeginXmit | txBaudMatch | ~ctlQ}});

// Baud rate tracking (RX).
assign rxBaudMatch = ~(|(rxBaudQ ^ brgQ));
assign rxTakeSample = ~(|(rxBaudQ[14:0] ^ brgQ[15:1]));
assign rxBaudIncA = rxBaudQ;
assign rxBaudD = rxBaudIncY & ~({16{rxBeginReceive | rxBaudMatch | ~ctlQ}});

// Bits sent tracking (TX).
assign txBitsD = txBitsIncY & ~{4{txBeginXmit | ~ctlQ}};	// ctlQ == 0 when uart disabled
assign txBitsEn = txBaudMatch | txBeginXmit;
assign txBitsIncA = txBitsQ;
assign txLastBit = txBitsQ[3] & ~txBitsQ[2] & ~txBitsQ[1] & txBitsQ[0]; // 9 bits sent (and on 10th)

// Bits received tracking (RX).
assign rxBitsD = rxBitsIncY & ~{4{rxBeginReceive | ~ctlQ}};	// ctlQ == 0 when uart disabled
assign rxBitsEn = rxBaudMatch | rxBeginReceive;
assign rxBitsIncA = rxBitsQ;
assign rxLastBit = rxBitsQ[3] & ~rxBitsQ[2] & ~rxBitsQ[1] & rxBitsQ[0]; // 9 bits sent (and on 10th)

// Output/Status bits (TX).
assign txDoneD = txStateQ & txLastBit & txBaudMatch;
assign txOutD = (txrQ[0] | ~txStateD) & ctlQ;

// Output/Status bits (RX).
assign rxDoneD = rxStateQ & rxLastBit & rxBaudMatch;
assign rxNewD = rxStateQ & ~rxStateD;

// Tristate to bus line.
mux2 iMUX1[15:0] (.A(ctlQ_SW), .B(brgQ), .sel(busAddr[0]), .out(brgCtl));
mux2 iMUX2[15:0] (.A(rxrQ_SW), .B(txrQ_SW), .sel(busAddr[0]), .out(txrRxr));
mux2 iMUX3[15:0] (.A(txrRxr), .B(brgCtl), .sel(busAddr[1]), .out(triA));
assign triEn = busEn & ~busWr;

// Drive outputs.
assign busData = triY;
assign txOut = txOutQ;
assign sigTxInt = txDoneQ;
assign sigRxInt = rxDoneQ;

// TODO- test signals, remove
assign T_a0 = brgQ;
assign T_a1 = {15'b0, ctlQ};
assign T_a2 = {8'b0, rxBitsQ, 3'b0, rxStateQ}; //{6'b0, txrQ};
assign T_a3 = {6'b0, rxrQ};

endmodule
