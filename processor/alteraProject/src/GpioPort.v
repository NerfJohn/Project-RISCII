module GpioPort (
	// Memory Bus Connections.
	input[1:0] GPort_busAddr,
	input       GPort_busWr,
	input       GPort_busEn,
	inout[15:0] GPort_busData,
	
	// External Pin Connections.
	inout[15:0] GPort_extPin,
	
	// TODO-remove.
	output[15:0] test_tris,
	
	// Common Nets/Signals.
	input       clk,
	input       rstn
);

/////////////////////////
// -- Signals/Wires -- //
/////////////////////////

// Memory-mapped register wires.
wire[15:0] triD, triQ;
wire triEn;
wire[15:0] outD, outQ;
wire outEn;
wire[15:0] inpD, inpQ;

// Pin tristate/gate wires.
wire[15:0] pinGateA, pinGateY;
wire pinGateEn;

// Memory bus tristate/gate wires.
wire[15:0] memGateA, memGateY;
wire memGateEn;

// Wires/logic to determine register to read/write.
wire isAddrTRI, isAddrOUT, isAddrINP;

// Wires/logic to determine data to read back.
wire[15:0] muxData_0X, muxData_result;

////////////////////////////
// -- Blocks/Instances -- //
////////////////////////////

// Memory-mapped R/W GPIO Registers.
dff_en TRI[15:0] (
	.D(triD), 
	.Q(triQ), 
	.en(triEn), 
	.clk(clk), 
	.rstn(rstn)
);
dff_en OUT[15:0] (
	.D(outD), 
	.Q(outQ), 
	.en(outEn), 
	.clk(clk), 
	.rstn(rstn)
);

// Synchronizers (ie memory-mapped Read-only GPIO register).
SynchFlop INP[15:0] (
	.D(inpD),
	.Q(inpQ),
	.clk(clk),
	.rstn(rstn)
);

// Tristate to drive pin outputs.
Tristate PIN_GATE[15:0] (
	.A(pinGateA),
	.Y(pinGateY),
	.en(pinGateEn)
);

// Tristate to drive memory reads.
Tristate MEM_GATE[15:0] (
	.A(memGateA),
	.Y(memGateY),
	.en(memGateEn)
);

//////////////////////////
// -- Connects/Logic -- //
//////////////////////////

// Determine which register is being accessed.
assign isAddrTRI = ~GPort_busAddr[1] & ~GPort_busAddr[0]; // Addr 0b00
assign isAddrOUT = ~GPort_busAddr[1] & GPort_busAddr[0]; // Addr 0b01
assign isAddrINP = GPort_busAddr[1] & ~GPort_busAddr[0]; // Addr 0b10

// TRI and OUT registers drive external pins.
assign pinGateA = outQ;
assign pinGateEn = triQ;

// Tie external pins to in/out wires of port.
assign inpD = GPort_extPin;
assign GPort_extPin = pinGateY;

// Allow TRI register to be written.
assign triD = GPort_busData;
assign triEn = isAddrTRI & GPort_busEn & GPort_busWr;

// Allow OUT register to be written.
assign outD = GPort_busData;
assign outEn = isAddrOUT & GPort_busEn & GPort_busWr;

// Determine what data to read back.
mux2 MUX0[15:0] (.A(outQ), .B(triQ), .sel(GPort_busAddr[0]), .out(muxData_0X));
mux2 MUX1[15:0] (.A(inpQ), .B(muxData_0x), .sel(GPort_busAddr[1]), .out(muxData_result));

// Read back data for memory reads.
assign memGateA = muxData_result;
assign memGateEn = GPort_busEn & ~GPort_busWr;
assign GPort_busData = memGateY;

// TODO- remove.
assign test_tris = triQ;

endmodule
