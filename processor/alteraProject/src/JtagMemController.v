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

// JTAG control/data wires (exlcuding external-facing wires).
wire jtag_wrData, jtag_doUpdate;
wire[7:0] jtag_instrLine;

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
			 .wrData(jtag_wrData), .doUpdate(jtag_doUpdate), .instrLine(jtag_instrLine), .dataLine(data),
			 .clk(clk), .rstn(rstn),
			 .test_state(test_state),
			 .test_data(test_data)
);

// Address register (head of address portion of bus- written by JTAG port).
dff_en ADDR_REG[15:0] (.D(addrD), .Q(addrQ), .en(addrEn), .clk(clk), .rstn(rstn));

// Control registers of memory bus.
myDff WR (.D(wrD), .Q(wrQ), .clk(clk), .rstn(rstn));
myDff EN (.D(enD), .Q(enQ), .clk(clk), .rstn(rstn));

// Register to create MCU "pulse" for running command.
myDff PULSE (.D(pulseD), .Q(pulseQ), .clk(clk), .rstn(rstn));

//////////////////////////
// -- Connects/Logic -- //
//////////////////////////

// "Pulser" signals MCU when to apply command.
assign cmdPulse = jtag_doUpdate & ~pulseQ;
assign pulseD = jtag_doUpdate;

// Set address register on "address" command.
assign addrD = data;
assign addrEn = ~jtag_instrLine[1] & jtag_instrLine[0] & cmdPulse; // addr cmd = 0x01

// Set memory bus controls.
assign wrD = jtag_instrLine[0]; // write cmd = 0x03
assign enD = jtag_instrLine[1] & jtag_doUpdate; // read/write cmds = 0b0000001X

// Overwrite JTAG data register for memory read.
assign jtag_wrData = enQ & ~wrQ & jtag_doUpdate; // memory read

// Test/Debug signals.
assign test_instr = jtag_instrLine;
assign test_addr = addrQ;
assign test_sigs = {enQ, wrQ};

// Drive outputs.
assign addr = addrQ;
assign isWr = wrQ;
assign memEn = enQ & jtag_doUpdate;

endmodule
