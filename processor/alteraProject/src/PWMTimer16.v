module PWMTimer16(busAddr, busData, busEn, busWr,	// Bus Signals
					   sigIntr, sigCmp,						// Special Signals
					   clk, rstn,								// Common Signals
						T_val, T_ctl, T_max, T_cmp       // TODO- test signals, remove
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
output sigIntr;			// Interrupt signal (for overflow)
output sigCmp;          // Comparison result (VAL >= CMP)

// Common Signals.
input clk, rstn;			// Common clock and active-low reset

// TODO- test signals, remove.
output[15:0] T_val, T_ctl, T_max, T_cmp;

/////////////////////////
// -- Signals/Wires -- //
/////////////////////////

// Bus-Accessible Register Wires (Value, Control, Max Count).
wire[15:0] valD, valQ;
wire[4:0]  ctlD, ctlQ;          // 16-bit register, ranges [7:4] and [0]
wire[15:0] maxD, maxQ;
wire[15:0] cmpD, cmpQ;
wire       valEn, ctlEn, maxEn, cmpEn;

// Hidden Register Wires (Interrupt and Prescale).
wire[3:0]  psD, psQ;
wire       intD, intQ;
wire       cprD, cprQ;

// Incrementor Wires (for Value and Prescale).
wire[15:0] inc16A, inc16Y;
wire[3:0]  inc4A, inc4Y;

// Comparator: Compares for A >= B (assuming B is negated).
wire[15:0] gtA, gtB;
wire       gtAgteB;

// Tristate Wires (for data line during read).
wire[15:0] triA, triY;
wire       triEn;
wire[15:0] valCtl, maxCmp;		  // Mid-mux values (resolve addr).

// Additional signals.
wire psMatch, maxMatch;                  // Status of Value/Prescale comparisons
wire valAddr, ctlAddr, maxAddr, cmpAddr; // Status of address being input
wire valWr;                              // Status of Value being directly written
wire regWr;                              // Status of bus writing a register
wire[15:0] ctlSW;				              // Control register in "SW" view format

////////////////////////////
// -- Blocks/Instances -- //
////////////////////////////

// Bus-Accessible Registers.
dff_en VAL[15:0] (.D(valD), .Q(valQ), .en(valEn), .clk(clk), .rstn(rstn));
dff_en CTL[4:0]  (.D(ctlD), .Q(ctlQ), .en(ctlEn), .clk(clk), .rstn(rstn));
dff_en MAX[15:0] (.D(maxD), .Q(maxQ), .en(maxEn), .clk(clk), .rstn(rstn));
dff_en CMP[15:0] (.D(cmpD), .Q(cmpQ), .en(cmpEn), .clk(clk), .rstn(rstn));

// Hidden Registers.
myDff PS[3:0] (.D(psD), .Q(psQ), .clk(clk), .rstn(rstn));
myDff INT     (.D(intD), .Q(intQ), .clk(clk), .rstn(rstn));
myDff CPR     (.D(cprD), .Q(cprQ), .clk(clk), .rstn(rstn));

// Incrementors.
add_16b INC16 (.src1(inc16A), .src2(16'b0), .cin(1'b1), .sum(inc16Y), .cout(/*NC*/));
add_4b  INC4  (.A(inc4A), .B(4'b0), .Cin(1'b1), .S(inc4Y), .Cout(/*NC*/));

// Comparator.
add_16b GT16  (.src1(gtA), .src2(~gtB), .cin(1'b1), .sum(/*NC*/), .cout(gtAgteB));

// Tristate.
Tristate TRI[15:0] (.A(triA), .en(triEn), .Y(triY));

//////////////////////////
// -- Connects/Logic -- //
//////////////////////////

// Resolve additional signals (ease of reading logic).
assign psMatch = ~(|(psQ ^ ctlQ[4:1]));
assign maxMatch = ~(|(maxQ ^ valQ));
assign valAddr = ~busAddr[0] & ~busAddr[1];	// Value Addr = 0x00
assign ctlAddr = busAddr[0] & ~busAddr[1];	// Control Addr = 0x01
assign maxAddr = ~busAddr[0] & busAddr[1];   // Max Addr = 0x10
assign cmpAddr = busAddr[0] & busAddr[1];    // Max Addr = 0x11
assign valWr = regWr & valAddr;
assign regWr = busEn & busWr;
assign ctlSW = {8'b0,ctlQ[4:1], 3'b0, ctlQ[0]};

// Bus-Accessible Registers (value chooses between inc/roll and write).
mux2 iMUX0[15:0] (.A(busData), .B(inc16Y & {16{~maxMatch}}), .sel(valWr), .out(valD));
assign ctlD = {busData[7:4], busData[0]};
assign maxD = busData;
assign cmpD = busData;
assign valEn = (ctlQ[0] & psMatch) | valWr;
assign ctlEn = regWr & ctlAddr;
assign maxEn = regWr & maxAddr;
assign cmpEn = regWr & cmpAddr;

// Interrupt/Prescale Registers.
assign psD = inc4Y & {4{~(psMatch | regWr)}};
assign intD = ctlQ[0] & maxMatch & psMatch;
assign cprD = ctlQ[0] & gtAgteB;

// Incrementors.
assign inc16A = valQ;
assign inc4A = psQ;

// Comparator (A >= B).
assign gtA = valQ;
assign gtB = cmpQ;

// Tristate.
mux2 iMUX1[15:0] (.A(ctlSW), .B(valQ), .sel(busAddr[0]), .out(valCtl));
mux2 iMUX2[15:0] (.A(cmpQ), .B(maxQ), .sel(busAddr[0]), .out(maxCmp));
mux2 iMUX3[15:0] (.A(maxCmp), .B(valCtl), .sel(busAddr[1]), .out(triA));
assign triEn = busEn & ~busWr;

// Drive outputs.
assign busData = triY;
assign sigIntr = intQ;
assign sigCmp = cprQ;

// TODO- test signals, remove.
assign T_val = valQ;
assign T_ctl = ctlSW;
assign T_max = maxQ;
assign T_cmp = cmpQ;

endmodule
