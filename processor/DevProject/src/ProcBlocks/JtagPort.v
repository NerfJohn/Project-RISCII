/*
 * JtagPort.v
 *
 * "Circuit to parse JTAG signals into requests and control signals"
 */
module JtagPort (
	// JTAG pin connections.
	input         i_TCK,
	input         i_TMS,
	input         i_TDI,
	output        o_TDO,
	
	// Current state of the uP.
	input         i_isBooted,
	input         i_isPaused,
	
	// TODO- remove/refactor.
	output [15:0] o_addr,
	
	// Common signals.
	input         i_rstn
);

/*
 * TODO- desc.
 */
 
////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// State Machine wires.
wire [2:0]  stateD, stateQ;
wire [2:0]  nextState1XX, nextState0XX;
wire [2:0]  nextState10X, nextState00X;

// Computed controls (based on state machine).
wire        inDSHFT;
wire        loadStatus;

// Shift Registers wires.
wire [7:0]  cmdD, cmdQ;
wire [15:0] dataD, dataQ;
wire        dataEn;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// State Machine- controlled by JTAG pins, controls overall port.
DffAsynch STATE[2:0] (
	.D(stateD),
	.Q(stateQ),
	.clk(i_TCK),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Shift registers- directly affected by JTAG pin inputs.
DffAsynch CMD[7:0] (
	.D(cmdD),
	.Q(cmdQ),
	.clk(i_TCK),
	.rstn(i_rstn)
);
DffAsynchEn DATA[15:0] (
	.D(dataD),
	.Q(dataQ),
	.S(dataEn),
	.clk(i_TCK),
	.rstn(i_rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Determine next state of JTAG/state machine.
Mux2 M0[2:0] (
	.A({1'b0, ~i_TMS, 1'b0}), // D-SEL  (101)? To D-SHFT (010) or IDLE (000)
	.B({1'b1, i_TMS, 1'b0}),  // I-SHFT (100)? To I-SHFT (100) or UPDATE (110)
	.S(stateQ[0]),
	.Y(nextState10X)
);
Mux2 M1[2:0] (
	.A({1'b0, 1'b0, 1'b0}),   // UPDATE (11X)? To IDLE (000)
	.B(nextState10X),         // No     (10X)? Process bit 0
	.S(stateQ[1]),
	.Y(nextState1XX)
);
Mux2 M2[2:0] (
	.A({1'b1, 1'b0, i_TMS}),  // I-SEL (001)? To I-SHFT (100) or D-SEL (101)
	.B({1'b0, 1'b0, ~i_TMS}), // IDLE  (000)? To I-SEL  (001) or IDLE  (000)
	.S(stateQ[0]),
	.Y(nextState00X)
);
Mux2 M3[2:0] (
	.A({1'b0, ~i_TMS, 1'b0}), // D-SHFT (01X)? To D-SHFT (010) or IDLE (000)
	.B(nextState00X),         // No     (00X)? Process bit 0
	.S(stateQ[1]),
	.Y(nextState0XX)
);
Mux2 M4[2:0] (
	.A(nextState1XX),         // Bit 2 set? Process bit 1 as 1XX
	.B(nextState0XX),         // No?        Process bit 1 as 0XX
	.S(stateQ[2]),
	.Y(stateD)
);

//------------------------------------------------------------------------------
// Compute state machine based controls.
assign inDSHFT    = ~stateQ[2] & stateQ[1];
assign loadStatus = ~stateQ[2] & ~stateQ[1] & stateQ[0] & ~i_TMS;

//------------------------------------------------------------------------------
// Handle shift registers- shifted vs set inputs.
Mux2 M5[7:0] (
	.A({6'b000000, i_isPaused, i_isBooted}), // Enter ISHFT? Load uP Status
	.B({cmdQ[6:0], i_TDI}),                  // No?          Left shift in TDI
	.S(loadStatus),
	.Y(cmdD)
);
assign dataD  = {dataQ[14:0], i_TDI};
assign dataEn = inDSHFT;


//------------------------------------------------------------------------------
// TODO- remove/refactor.
assign o_addr = {cmdQ, dataQ[7:0]};
Mux2 M99 (
	.A(dataQ[15]),
	.B(cmdQ[7]),
	.S(inDSHFT),
	.Y(o_TDO)
);

endmodule
