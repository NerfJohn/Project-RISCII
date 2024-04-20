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
	
	// External SPI control pins.
	output spiControl,
	output spiClk,
	output spiTdi,
	input  spiTdo,
	
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

/*
Commands:
0x00 = pass/no-op
0x01 = set address register
0x02 = read memory
0x03 = write memory
0x04 = set data reg to SPI mode
*/

/////////////////////////
// -- Signals/Wires -- //
/////////////////////////

// Synchronizer wires.
wire[2:0] synchD, synchQ;
wire tckSynch, tmsSynch, tdiSynch;

// JTAG control/data wires (exlcuding external-facing wires).
wire jtag_wrData, jtag_doUpdate, jtag_inDShift, jtag_Tdo;
wire[7:0] jtag_instrLine;

// Address register wires.
wire[15:0] addrD, addrQ;
wire addrEn;

// SPI Mode register wires.
wire[15:0] spiD, spiQ;
wire spiEn;

// Was instruction reg.
wire isInstrD, isInstrQ;

// Control register wires.
wire wrD, wrQ;
wire enD, enQ;

// "Pulser" wires.
wire pulseD, pulseQ;
wire cmdPulse;

////////////////////////////
// -- Blocks/Instances -- //
////////////////////////////

// Synchronizer for external inputs.
SynchFlop SYNCH[2:0] (.D(synchD), .Q(synchQ), .clk(clk), .rstn(rstn));

// JTAG port.
JtagPort JTAG(.tckSynch(tckSynch), .tmsSynch(tmsSynch), .tdiSynch(tdiSynch), .tdo(jtag_Tdo), 
			 .wrData(jtag_wrData), .doUpdate(jtag_doUpdate), .instrLine(jtag_instrLine), .dataLine(data),
			 .inDShift(jtag_inDShift),
			 .clk(clk), .rstn(rstn),
			 .test_state(),
			 .test_data(test_data)
);

// Address register (head of address portion of bus- written by JTAG port).
dff_en ADDR_REG[15:0] (.D(addrD), .Q(addrQ), .en(addrEn), .clk(clk), .rstn(rstn));

// Bit to track if last state was instruction state (ie cmdPulse trigger).
myDff WAS_INSTR (.D(isInstrD), .Q(isInstrQ), .clk(tckSynch), .rstn(rstn));

// "SPI" mode register- used to distinguish when to pass signals to SPI port.
dff_en SPI_REG (.D(spiD), .Q(spiQ), .en(spiEn), .clk(clk), .rstn(rstn));

// Control registers of memory bus.
myDff WR (.D(wrD), .Q(wrQ), .clk(clk), .rstn(rstn));
myDff EN (.D(enD), .Q(enQ), .clk(clk), .rstn(rstn));

// Register to create MCU "pulse" for running command.
myDff PULSE (.D(pulseD), .Q(pulseQ), .clk(clk), .rstn(rstn));

//////////////////////////
// -- Connects/Logic -- //
//////////////////////////

// Pass external inputs through synchronizing.
assign synchD = {tck, tms, tdi};
assign tckSynch = synchQ[2];
assign tmsSynch = synchQ[1];
assign tdiSynch = synchQ[0];

// "Pulser" signals MCU when to apply command.
assign cmdPulse = jtag_doUpdate & ~pulseQ;
assign pulseD = jtag_doUpdate;

// Set address register on "address" command.
assign addrD = data;
assign addrEn = ~jtag_instrLine[1] & jtag_instrLine[0] & cmdPulse; // addr cmd = 0x01

// Set SPI mode based on "spi" command.
assign spiD = jtag_instrLine[2]; // spi cmd = 0x04 (or just 3'b1XX)
assign spiEn = cmdPulse & isInstrQ;

// Determine if last state was instruction (based on DShift).
assign isInstrD = ~jtag_inDShift;

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

assign test_instr = jtag_instrLine;

// Output desired SPI controls.
assign spiControl = spiQ & jtag_inDShift;
assign spiClk = tckSynch;
assign spiTdi = tdiSynch;
mux2 MUX0 (.A(spiTdo), .B(jtag_Tdo), .sel(spiQ & jtag_inDShift), .out(tdo));

endmodule
