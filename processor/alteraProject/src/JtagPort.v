module JtagPort (
	// External-facing input pins.
	input        tck,
	input        tms,
	input        tdi,
	output       tdo,
	
	// MCU-facing I/O pins.
	input        wrData,
	output       doUpdate,
	output[7:0]  instrLine,
	inout[15:0]  dataLine,
	
	// Common signals.
	input        clk,
	input        rstn,
	
	// TODO- remove test signals.
	output[2:0]  test_state,
	output[15:0] test_data
);

/*
General States:
	Idle     (3'b000): Reset/Waiting state
	Select-I (3'b100): Confirm instruction selection state
	Select-D (3'b110): Confirm data selection state
	Shift-I  (3'b101): Shift in instruction state
	Shift-D  (3'b111): Shift in data state
	Update   (3'b001): Update MCU
	
	TMS = 1 generally moves towards "Idle" state
	TMS = 0 generally moves towards "Shift" states
	
	"Update" clears instruction register on exit
		(assumes instruction = 0 is a "nop" command)
*/

/////////////////////////
// -- Signals/Wires -- //
/////////////////////////

// Synchronizer wires.
wire[2:0] synchD, synchQ;
wire tckSynch, tmsSynch, tdiSynch;

// State machine wires.
wire[2:0] stateD, stateQ;

// Instruction/Data register wires.
wire[7:0]  instrD, instrQ;
wire[15:0] dataD, dataQ;
wire instrEn, dataEn;
wire[15:0] dataShifted;

// Data line Tristate wires.
wire[15:0] triA, triY;
wire triEn;

// Pertinent states for operation.
wire inShiftD, inUpdate;

// Transitional states for determining next state.
wire[2:0] state100Next, state101Next;
wire[2:0] state110Next, state111Next;
wire[2:0] state00XNext;
wire[2:0] mux_100_101, mux_110_111, mux_10X_11X;

////////////////////////////
// -- Blocks/Instances -- //
////////////////////////////

// Synchronizer for external inputs.
SynchFlop SYNCH[2:0] (.D(synchD), .Q(synchQ), .clk(clk), .rstn(rstn));

// State of the JTAG port.
myDff STATE[2:0] (.D(stateD), .Q(stateQ), .clk(tckSynch), .rstn(rstn));

// Official Instruction/Data registers of port- "read only" to MCU.
dff_en INSTR[7:0] (.D(instrD), .Q(instrQ), .en(instrEn), .clk(tckSynch), .rstn(rstn));
dff_en DATA[15:0] (.D(dataD), .Q(dataQ), .en(dataEn), .clk(tckSynch), .rstn(rstn));

// Tristate controlling "direction" of data register read/write.
Tristate TRI[15:0] (.A(triA), .en(triEn), .Y(triY));

//////////////////////////
// -- Connects/Logic -- //
//////////////////////////

// Get "is state" signals.
assign inShiftD =  stateQ[2] &  stateQ[1] &  stateQ[0]; // 3'b111
assign inUpdate = ~stateQ[2] & ~stateQ[1] &  stateQ[0]; // 3'b001

// Pass external inputs through synchronizing.
assign synchD = {tck, tms, tdi};
assign tckSynch = synchQ[2];
assign tmsSynch = synchQ[1];
assign tdiSynch = synchQ[0];

// Determine next state (for each state).
assign state100Next = {1'b1, tmsSynch, ~tmsSynch};
assign state101Next = {~tmsSynch, 2'b01};
assign state110Next = {{3{~tmsSynch}}};
assign state111Next = {{2{~tmsSynch}}, 1'b1};
assign state00XNext = {~tmsSynch, 2'b00};

// Determine next state (for overall port).
mux2 iMUX0[2:0] (.A(state101Next),   .B(state100Next),   .sel(stateQ[0]), .out(mux_100_101));
mux2 iMUX1[2:0] (.A(state111Next),   .B(state110Next),   .sel(stateQ[0]), .out(mux_110_111));
mux2 iMUX2[2:0] (.A(mux_110_111),    .B(mux_100_101),    .sel(stateQ[1]), .out(mux_10X_11X));
mux2 iMUX3[2:0] (.A(mux_10X_11X),    .B(state00XNext),   .sel(stateQ[2]), .out(stateD));

// Handle shifting into instruction register.
assign instrEn = ~stateQ[1] & stateQ[0];
assign instrD = {8{~inUpdate}} & {tdiSynch, instrQ[7:1]};

// Handle shifting into data register.
assign dataEn = inShiftD | wrData;
assign dataShifted = {tdiSynch, dataQ[15:1]};
mux2 iMUX5[15:0] (.A(dataLine), .B(dataShifted), .sel(wrData), .out(dataD));

// Tristate data register lines.
assign triA = dataQ;
assign triEn = ~wrData;

// Set output signals to MCU.
assign instrLine = instrQ;
assign dataLine = triY;
assign doUpdate = inUpdate;

// Output shifting register.
mux2 iMUX4 (.A(dataQ[0]), .B(instrQ[0]), .sel(stateQ[1]), .out(tdo));

// TODO- remove as needed.
assign test_state = stateQ;
assign test_data = dataQ;

endmodule
