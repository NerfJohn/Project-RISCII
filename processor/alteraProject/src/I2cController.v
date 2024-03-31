module I2cController (
	// Memory Bus Connections.
	input[1:0]  I2cController_busAddr,
	input       I2cController_busWr,
	input       I2cController_busEn,
	inout[15:0] I2cController_busData,
	
	// I2C pins/signals.
	output      I2cController_SCL,
	inout       I2cController_SDA,
	
	output[5:0] test_state,
	output test_data,
	
	// Common Nets/Signals.
	input       clk,
	input       rstn
);

/*
	== Basic I2c Controller ==
	Addr 0x0: [16'hAAAA]
		-> A (r/w): Baud rate for SCL, LSb ignored/assumed 0
	Addr 0x1: [9'hXXX][A][B][C][D][E][F][G]
		-> A (r)  : Current state of SCL
		-> B (r)  : Set for finished operation/idle
		-> C (r)  : ACK result from last operation
		-> D (r/w): Set for generate START on next operation
		-> E (r/w): Set for generate STOP on next operation
		-> F (r/w): Set to begin read operation (if idle)
		-> G (r/w): Set if I2C Controller is enabled
	Addr 0x2: [8'hXX][8'hAA]
		-> A (r/w): I2C data, write while enabled and idle triggers operation
*/

/////////////////////////
// -- Signals/Wires -- //
/////////////////////////

// Wires to memory mapped registers.
wire[14:0] baudRateD, baudRateQ;
wire baudRateEn;
wire ctlStartD, ctlStopD, ctlWrD, ctlEnD;
wire ctlStartQ, ctlStopQ, ctlWrQ, ctlEnQ;
wire ctlEn;
wire[7:0] dataD, dataQ;
wire dataEn;

// Extra wires/signals of memory bus operations.
wire isBaudAddr, isCtlAddr, wrDataAddr;
wire[15:0] mux_baud_ctl;

// Wires to memory bus driving tristate.
wire[15:0] triA, triY;
wire triEn;

// Wires to baud tracking pieces.
wire[14:0] baudD, baudQ;
wire[15:0] baudIncA, baudIncY;
wire baudMatch;

// Wires to state registers/blocks.
wire[4:0] stateD, stateQ;
wire stateEn;

wire runD, runQ, runEn;

wire[4:0] stateIncA, stateIncY;
wire[4:0] stateIncMid;

// Wires to tristate driving SDA line.
wire sdaTriA, sdaTriY, sdaTriEn;

// Processed signals useful for state actions.
wire beginOp, endOp;

// Processed signals useful for SCL/SDA controls.
wire inStart, inStop, inAck, inData;
wire mux_clk_start, mux_sdaStart_sdaStop;

// Wires to ACK/NAK receiver.
wire recD, recQ, recEn;

////////////////////////////
// -- Blocks/Instances -- //
////////////////////////////

// Memory mapped registers.
dff_en BAUD_RATE[14:0] (
	.D(baudRateD),
	.Q(baudRateQ),
	.en(baudRateEn),
	.clk(clk),
	.rstn(rstn)
);

dff_en CTL[3:0] (
	.D({ctlStartD, ctlStopD, ctlWrD, ctlEnD}),
	.Q({ctlStartQ, ctlStopQ, ctlWrQ, ctlEnQ}),
	.en(ctlEn),
	.clk(clk),
	.rstn(rstn)
);

dff_en DATA[7:0] (
	.D(dataD),
	.Q(dataQ),
	.en(dataEn),
	.clk(clk),
	.rstn(rstn)
);

// Memory Bus driving tristate.
Tristate MEM_TRI[15:0] (
	.A(triA),
	.en(triEn),
	.Y(triY)
);

// Register/Adder creating baud pulses (for internal use).
myDff BAUD[14:0] (
	.D(baudD),
	.Q(baudQ),
	.clk(clk),
	.rstn(rstn)
);

add_16b BAUD_INC (
	.src1({1'b0, baudIncA}),
	.src2(1'b0),
	.cin(1'b1),
	.sum(baudIncY),
	.cout(/*NC*/)
);

// State registers/incrementor.
dff_en STATE[4:0] (
	.D(stateD),
	.Q(stateQ),
	.en(stateEn),
	.clk(clk),
	.rstn(rstn)
);

dff_en RUNNING (
	.D(runD),
	.Q(runQ),
	.en(runEn),
	.clk(clk),
	.rstn(rstn)
);

add_1b STATE_INC[4:0] (
	.A(stateIncA),
	.B(1'b0),
	.Cin({stateIncMid[3:0], 1'b1}),
	.S(stateIncY),
	.Cout(stateIncMid)
);

// Tristate driving SDA line.
Tristate SDA_TRI (
	.A(sdaTriA),
	.en(sdaTriEn),
	.Y(sdaTriY)
);

// Record register for ACK/NAK signal.
dff_en REC_ACK (
	.D(recD),
	.Q(recQ),
	.en(recEn),
	.clk(clk),
	.rstn(rstn)
);

//////////////////////////
// -- Connects/Logic -- //
//////////////////////////

// Determine memory bus signals.
assign isBaudAddr = ~I2cController_busAddr[1] & ~I2cController_busAddr[0];
assign isCtlAddr  = ~I2cController_busAddr[1] & I2cController_busAddr[0];
assign wrDataAddr = I2cController_busWr & I2cController_busEn & 
							I2cController_busAddr[1] & ~I2cController_busAddr[0];

// Connect/write to memory-mapped registers.
assign baudRateD  = I2cController_busData[15:1];
assign baudRateEn = I2cController_busWr & I2cController_busEn & isBaudAddr;
assign ctlStartD  = I2cController_busData[3];
assign ctlStopD   = I2cController_busData[2];
assign ctlWrD     = I2cController_busData[1];
assign ctlEnD     = I2cController_busData[0];
assign ctlEn = I2cController_busWr & I2cController_busEn & isCtlAddr;
mux2 iMUX4[7:0] (
	.A(I2cController_busData[7:0]),
	.B({dataQ[6:0], I2cController_SDA}),
	.sel(wrDataAddr),
	.out(dataD)
);
assign dataEn = wrDataAddr | (inData & baudMatch & runQ & stateQ[0]);

// Drive bus with read hardware register.
mux2 iMUX0[15:0] (
	.A({11'h0, recQ, ctlStartQ, ctlStopQ, ctlWrQ, ctlEnQ}),
	.B({baudRateQ, 1'h0}),
	.sel(I2cController_busAddr[0]),
	.out(mux_baud_ctl)
);
mux2 iMUX1[15:0] (
	.A({8'h0, dataQ}),
	.B(mux_baud_ctl),
	.sel(I2cController_busAddr[1]),
	.out(triA)
);
assign triEn = I2cController_busEn & ~I2cController_busWr;
assign I2cController_busData = triY;

// Track/control baud rate of clock.
assign baudMatch = ~(|(baudIncY ^ baudRateQ));
assign baudIncA = {1'b0, baudQ};
assign baudD = baudIncY[14:0] & ~{14{dataEn | baudMatch}};

// Manage state machine transitions.
assign beginOp = ctlEnQ & ~runQ & dataEn;
assign endOp = stateQ[4] & ~stateQ[3] & stateQ[2] & ~stateQ[1] & stateQ[0] & baudMatch; // states 0 - 21, end on 21.

assign runD = beginOp & ~endOp;
assign runEn = beginOp | endOp;

assign stateIncA = stateQ;
assign stateD = stateIncY & ~{5{endOp}};
assign stateEn = runQ & baudMatch;

// SCL control- state gives "clk" signal/idle signal, start/stop override as needed.
assign inStart = ~stateQ[4] & ~stateQ[3] & ~stateQ[2] & ~stateQ[1] & runQ; // States 5'b0000X and running
assign inStop = stateQ[4] & ~stateQ[3] & stateQ[2] & ~stateQ[1]; // States 5'b1010X

mux2 iMUX2 (
	.A(ctlStartQ),
	.B(stateQ[0]),
	.sel(inStart),
	.out(mux_clk_start)
);
mux2 iMUX3 (
	.A(ctlStopQ),
	.B(mux_clk_start),
	.sel(inStop),
	.out(I2cController_SCL)
);

// SDA control- manage "shifting"/ack signal/etc.
assign inAck = stateQ[4] & ~stateQ[3] & ~stateQ[2] & stateQ[1]; // States 5'b1001X
assign inData = ~inStart & ~inAck & ~inStop;

mux2 iMUX5 (
	.A(inStart & ~stateQ[0]),
	.B(inStop & stateQ[0]),
	.sel(inStart),
	.out(mux_sdaStart_sdaStop)
);
mux2 iMUX6 (
	.A(mux_sdaStart_sdaStop),
	.B(dataQ[7] & ~inAck), // drive write data and read ACK
	.sel(inStop | inStart),
	.out(sdaTriA)
);
assign sdaTriEn = (inData & ctlWrQ) | (inAck & ~ctlWrQ) | (inStart & ctlStartQ) | (inStop & ctlStopQ);
//assign I2cController_SDA = sdaTriY;
myDff DELAY (
	.D(sdaTriY),
	.Q(I2cController_SDA),
	.clk(clk),
	.rstn(rstn)
);

// Record ACK/NAK result of operation.
assign recD = I2cController_SDA;
assign recEn = inAck & stateQ[0];

assign test_state = {runQ, stateQ};
assign test_data = sdaTriEn;

endmodule
