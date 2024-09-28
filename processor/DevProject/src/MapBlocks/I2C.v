/*
 * I2C.v
 *
 * "Mapped I2C controller capable of sending bytes with configurable overhead"
 */
module I2C (
	// Memory Map connections.
	input  [1:0]  i_memAddr,
	input  [15:0] i_memDataIn,
	input         i_memWrEn,
	output [15:0] o_memDataOut,
	
	// Serial pin connections.
	input         i_pinSDAIn,
	output        o_pinSCL,
	output        o_pinSDAOut,
	output        o_pinSDADir,

	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * TODO- desc
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Compute control wires (based on mem access/uP state).
wire        isCtrlWr, isBaudWr, isDataWr;

// CTRL register wires.
wire [3:0]  ctrlD, ctrlQ;
wire        ctrlS;
wire        cfgStart, cfgWr, cfgStop, cfgEnable;

// BAUD register wires.
wire [14:0] baudD, baudQ;
wire        baudS;

// DATA register wires.
wire [7:0]  dataD, dataQ;
wire        dataS;

// Compute control wires (based on core process).
wire        isIdle, isHead, isAck, isTail;
wire        atCntHalf, atCntMax;
wire        doStart, doShift;

// Core process wires.
wire [15:0] cntQ;
wire        cntRstn;
wire        halfD, halfQ, halfS;
wire [3:0]  addA, addB, addS;
wire        addI;
wire [3:0]  stateD, stateQ;
wire        stateS;

// SDA write wires.
wire        sdaD, sdaQ, sdaS;
wire        sdaNoTail, sdaAckTail;
wire        dirD, dirQ, dirS;
wire        dirNoTail;

// ACK capture wires.
wire        ackD, ackQ, ackS;

// Register read wires.
wire [15:0] rdCtrl;

// Pin drive wires.
wire        sclNoTail;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// CTRL register- writable controls for the I2C peripheral.
DffSynchEn CTRL[3:0] (
	.D(ctrlD),
	.Q(ctrlQ),
	.S(ctrlS),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// BAUD register- writable parameter defining 0.5 of I2C baud rate.
DffSynchEn BAUD[14:0] (
	.D(baudD),
	.Q(baudQ),
	.S(baudS),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// DATA register- register used to read/write data from I2C.
DffSynchEn DATA[7:0] (
	.D(dataD),
	.Q(dataQ),
	.S(dataS),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Core Process- pieces used to implement core state machine of I2C.
RegInc16 BAUD_COUNTER (
	.Q(cntQ),
	.clk(i_clk),
	.rstn(cntRstn)       // custom reset behavior
);
DffSynchEn HALF_PULSE (
	.D(halfD),
	.Q(halfQ),
	.S(halfS),
	.clk(i_clk),
	.rstn(i_rstn)
);
Add4 STATE_INC (
	.A(addA),
	.B(addB),
	.I(addI),
	.S(addS)
);
DffSynchEn STATE[3:0] (
	.D(stateD),
	.Q(stateQ),
	.S(stateS),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// SDA Out- controlled update of SDA write value.
DffSynchEn SDA_OUT (
	.D(sdaD),
	.Q(sdaQ),
	.S(sdaS),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn SDA_DIR (
	.D(dirD),
	.Q(dirQ),
	.S(dirS),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// ACK register- record ACK status of last message.
DffSynchEn ACK (
	.D(ackD),
	.Q(ackQ),
	.S(ackS),
	.clk(i_clk),
	.rstn(i_rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute mem access related control wires.
assign isCtrlWr = ~i_memAddr[1] & ~i_memAddr[0] & i_memWrEn; // ...00
assign isBaudWr = ~i_memAddr[1] &  i_memAddr[0] & i_memWrEn; // ...01
assign isDataWr =  i_memAddr[1] & ~i_memAddr[0] & i_memWrEn; // ...10

//------------------------------------------------------------------------------
// Handle CTRL register inputs.
assign ctrlD     = i_memDataIn[3:0];
assign ctrlS     = isCtrlWr;
assign cfgStart  = ctrlQ[3];         // bit 3
assign cfgWr     = ctrlQ[2];         // bit 2
assign cfgStop   = ctrlQ[1];         // bit 1
assign cfgEnable = ctrlQ[0];         // bit 0

//------------------------------------------------------------------------------
// Handle BAUD register inputs.
assign baudD = i_memDataIn[15:1];
assign baudS = isBaudWr;

//------------------------------------------------------------------------------
// Handle DATA register inputs.
Mux2 M0[7:0] (
	.A(i_memDataIn[7:0]),           // Mem Wr? use mem data
	.B({dataQ[6:0], i_pinSDAIn}),   // No?     use shift
	.S(isDataWr),
	.Y(dataD)
);
assign dataS = isDataWr | doShift; // Mem | sample

//------------------------------------------------------------------------------
// Compute core process related control wires.
assign isIdle    = ~(|stateQ);
assign isHead    = ~stateQ[3] & ~stateQ[2] & ~stateQ[1] &  stateQ[0];
assign isAck     =  stateQ[3] & ~stateQ[2] &  stateQ[1] & ~stateQ[0];
assign isTail    =  stateQ[3] & ~stateQ[2] &  stateQ[1] &  stateQ[0];
assign atCntHalf = ~(|(cntQ[14:0] ^ baudQ));
assign atCntMax  = ~(|(cntQ[15:1] ^ baudQ));
assign doStart   = cfgEnable & isIdle & isDataWr;
assign doShift   = ~isIdle & atCntHalf & halfQ & ~isHead & ~isAck & ~isTail;

//------------------------------------------------------------------------------
// Handle core process inputs.
assign cntRstn = i_rstn & ~isIdle & ~atCntMax;
assign halfD   = ~halfQ;                                 // oscillate...
assign halfS   = ~isIdle & atCntMax;                     // ...during process
assign addA    = stateQ;
assign addB    = 4'b0000;                                // using as inc
assign addI    = 1'b1;                                   // using as inc
assign stateD  = addS & ~{4{isTail & halfQ & atCntMax}};
assign stateS  = doStart | (~isIdle & halfQ & atCntMax); // 2 halves per state

//------------------------------------------------------------------------------
// Handle controlled SDA write inputs.
Mux2 M1 (
	.A(cfgStart & ~halfQ),          // Head state? use "start" logic
	.B(dataQ[7]),                   // No?         use "data" logic
	.S(isHead),
	.Y(sdaNoTail)
);
Mux2 M2 (
	.A(cfgStop),                    // Ack state?  use "ack" logic 
	.B(~cfgStop | halfQ),           // No?         use "stop" logic
	.S(isAck),
	.Y(sdaAckTail)
);
Mux2 M3 (
	.A(sdaAckTail),                 // Ack/Tail  ? process further
	.B(sdaNoTail),                  // No?         process further
	.S(isAck | isTail),
	.Y(sdaD)
);
assign sdaS = ~isIdle & atCntHalf; // Update twice per cycle in "mids"
Mux2 M4 (
	.A(cfgStart),                   // Head state? use "start" logic
	.B((cfgWr ^ isAck) & ~isIdle),  // No?         use default logic
	.S(isHead),
	.Y(dirNoTail)
);
Mux2 M5 (
	.A(cfgStop),                    // Tail state? use "stop" logic
	.B(dirNoTail),                  // No?         process further...
	.S(isTail),
	.Y(dirD)
);
assign dirS = ~isIdle & atCntHalf; // Update twice per cycle in "mids"

//------------------------------------------------------------------------------
// Handle ACK record inputs.
assign ackD = i_pinSDAIn;
assign ackS = isAck & atCntHalf & halfQ; // after ACK posedge

//------------------------------------------------------------------------------
// Drive mem data output based on given address.
assign rdCtrl = {isIdle,     // I2C idle status
					  ackQ,       // Last byte's ACK status
					  10'b0,
					  ctrlQ};     // Controls
Mux4 M6[15:0] (
	.C(rdCtrl),               // Address 00? Read controls/statuses
	.D({baudQ, 1'b0}),        // Address 01? Read baud rate
	.E({8'b00000000, dataQ}), // Address 10? Read data byte
	.F(16'b0000000000000000), // Address 11? no register
	.S(i_memAddr),
	.Y(o_memDataOut)
);

//------------------------------------------------------------------------------
// Drive I2C pin connections.
Mux2 M7 (
	.A(cfgStart & halfQ),             // Head state? use "start" logic
	.B(halfQ),                        // No?         use default logic
	.S(isHead),
	.Y(sclNoTail)
);
Mux2 M8 (
	.A(cfgStop & halfQ),              // Tail state? use "stop" logic
	.B(sclNoTail),                    // No?         process further...
	.S(isTail),
	.Y(o_pinSCL)
);
assign o_pinSDAOut = sdaQ;           // processed by controlling mechanism
assign o_pinSDADir = dirQ;           // synched w/ SDA write

endmodule
