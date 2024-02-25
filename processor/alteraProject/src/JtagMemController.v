module JtagMemController (
	// Memory bus wires (as controller).
	output[15:0] addr,
	inout[15:0]  data,
	output       isWr,
	output       memEn,
	
	// External JTAG pins.
	input  tck,
	input  tms,
	input  tdi,
	output tdo,
	
	// Common signals.
	input clk,
	input rstn,
	
	// TODO- remove.
	output[2:0]  test_state,
	output[7:0]  test_instr,
	output[15:0] test_data,
	output[15:0] test_addr,
	output[1:0]  test_sigs
	
);

/////////////////////////
// -- Signals/Wires -- //
/////////////////////////

// JTAG port wires (those NOT pass-throughs).
wire jtagWrData, jtagRunCmd;
wire[7:0] jtagIReg;
wire[15:0] jtagDReg;

// Address register wires.
wire[15:0] addrD, addrQ;
wire addrEn;

// Control register wires.
wire wrD, wrQ;
wire enD, enQ;

// "Pulser" wires.
wire pulseD, pulseQ;
wire cmdPulse;

////////////////////////////
// -- Blocks/Instances -- //
////////////////////////////

// JTAG port.
JtagPort JTAG(.tck(tck), .tms(tms), .tdi(tdi), .tdo(tdo), 
			 .wrData(jtagWrData), .doUpdate(jtagRunCmd), .instrLine(jtagIReg), .dataLine(jtagDReg),
			 .clk(clk), .rstn(rstn),
			 .test_state(test_state),
			 .test_data(test_data)
);

// Address register (head of address portion of bus- written by JTAG port).
dff_en ADDR[15:0] (.D(addrD), .Q(addrQ), .en(addrEn), .clk(clk), .rstn(rstn));

// Control registers of memory bus.
myDff WR (.D(wrD), .Q(wrQ), .clk(clk), .rstn(rstn));
myDff EN (.D(enD), .Q(enQ), .clk(clk), .rstn(rstn));

// Register to create MCU "pulse" for running command.
myDff PULSE (.D(pulseD), .Q(pulseQ), .clk(clk), .rstn(rstn));

//////////////////////////
// -- Connects/Logic -- //
//////////////////////////

// Create pulse for knowing when to run command.
assign pulseD = jtagRunCmd;
assign cmdPulse = pulseD & ~pulseQ;

// Write new address if instruction = 2'b01.
assign addrEn = ~jtagIReg[1] & jtagIReg[0] & cmdPulse;
assign addrD = jtagDReg;

// Set control registers (for mem opertions 10 and 11).
assign wrD = jtagIReg[0];
assign enD = jtagIReg[1] & cmdPulse; // one memory request per command

// Set JTAG for expected read/write memory operation.
assign jtagWrData = ~wrQ & enQ;

// Connect to memory bus.
assign addr = addrQ;
assign data = jtagDReg;
assign isWr = wrQ;
assign memEn = enQ;

// TODO- test signals, to delete.
assign test_instr = jtagIReg;

endmodule
