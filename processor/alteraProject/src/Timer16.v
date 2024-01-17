module Timer16(busAddr, busData, busEn, busWr,	// Bus Signals
					sigIntr,									// Special Signals
					clk, rstn,								// Common Signals
					TEST_val, TEST_ctrl, TEST_max		// Test Signals (TODO- Remove)
);

/*
	Timer16: Basic 16-bit timer with enable, scaling, and bus access.
	
	Features:
		-> Value Register (16b): Timer value, r/w at 0x0, reset to 0x0
		-> Control Register (8b): Timer controls, r/w at 0x1, reset to 0x0
			[0]: Enable bit- 1 = enable, 0 = disable
			[7:4]: Prescalar- 0 = 1:1, 1 = 2:1, 2 = 3:1, ... , 15 = 0xF
		-> Max Register (16b): Max timer value, r/w at 0x2, reset to 0xFFFF
		-> Interrupt Signal: High for MAX -> 0 (on tick where VAL = 0)
*/

// -- Signals/Wires -- //

// Bus Signals.
input 		busEn;		// Enable-  signals r/w of timer register
input 		busWr;		// Write-   specifies bus write (low = bus read)
input[1:0] 	busAddr;		// Address- register to access
inout[15:0] busData;		// Data-    passes read/write data

// Special Signals.
output sigIntr;			// Interrupt signal (for overflow)

// Common Signals.
input clk, rstn;			// Common clock and active-low reset

// TEST signals, TODO- remove
output[15:0] TEST_val, TEST_max, TEST_ctrl;

// Bus-Accessible Register Wires.
wire[15:0] valD, valQ;
wire[7:0]  ctlD, ctlQ;
wire[15:0] maxD, maxQ;
wire wrVal;
wire wrCtl;
wire wrMax;
wire[15:0] valInc;

// Value Compare Wires.
wire maxMatch;

// Prescale Logic Wires.
wire      psMatch;
wire[3:0] psNew;
wire[3:0] psD, psQ;

// Incrementor logic.
wire[15:0] incQ;

// Bus Data Control/Tristate Wires.
wire[15:0] mux0;
wire[15:0] triA;
wire		  triEn;
wire[15:0] triY;

// -- Circuits/Instances -- //

// Interrupt- remember/assert on edges.
myDff INTR (.D(maxMatch & ctlQ[0]), .Q(sigIntr), .clk(clk), .rstn(rstn));

// Bus-Accessible Registers.
dff_en VALR[15:0] (.D(valD), .Q(valQ), .en(wrVal), .clk(clk), .rstn(rstn));
dff_en CTLR[7:0]  (.D(ctlD), .Q(ctlQ), .en(wrCtl), .clk(clk), .rstn(rstn));
dff_en MAXR[15:0] (.D(maxD), .Q(maxQ), .en(wrMax), .clk(clk), .rstn(rstn));

// Prescale Logic.
add_4b INCP      (.A(psQ), .B(4'b0), .Cin(1'b1), .S(psNew), .Cout(/*NC*/));
myDff  PSCL[3:0] (.D(psD), .Q(psQ), .clk(clk), .rstn(rstn));

// Incrementor.
add_16b INCT (.src1(valQ), .src2(16'b0), .cin(1'b1), .sum(incQ), .cout(/*NC*/));

// Bus Data Line Control.
Tristate TRIS[15:0] (.A(triA), .en(triEn), .Y(triY));

// -- Raw Logic -- //

// Value compare- match current value to max register.
assign maxMatch = ~(|(valQ ^ maxQ));

// Prescale compare- count resets for match or bus write.
assign psMatch = ~(|(ctlQ[7:4] ^ psQ));
assign psD = psNew & {4{~(psMatch | (busEn & busWr))}};

// Set up data heading into registers- value comes from bus or internal logic.
assign valInc[15:0] = incQ & {16{~maxMatch}};
mux2 iMUX2[15:0] (.A(busData), .B(valInc), .sel(busEn & busWr & ~busAddr[1] & ~busAddr[0]), .out(valD));
assign ctlD = busData;
assign maxD = busData;

// Update registers as needed (bus write or incrementing).
assign wrVal = (ctlQ[0] & psMatch) | (busEn & busWr & ~busAddr[1] & ~busAddr[0]);
assign wrCtl = (busEn & busWr & ~busAddr[1] & busAddr[0]);
assign wrMax = (busEn & busWr & busAddr[1]);

// Control data line when reading timer registers.
mux2 iMUX0[15:0] (.A({8'b0,ctlQ}), .B(valQ), .sel(busAddr[0]), .out(mux0));
mux2 iMUX1[15:0] (.A(maxQ), .B(mux0), .sel(busAddr[1]), .out(triA));
assign triEn = busEn & ~busWr;
assign busData = triY;

// TEST signals- to remove.
assign TEST_val = valQ; //{psD, psNew, 5'b0, psMatch, busEn, busWr};
assign TEST_ctrl = ctlQ;
assign TEST_max = maxQ;

endmodule
