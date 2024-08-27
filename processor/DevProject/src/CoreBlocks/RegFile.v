/*
 * RegFile.v
 *
 * "Register file with eight 16-bit slots (one treated as stack pointer)"
 */
module RegFile (
	// Read connections.
	input  [2:0]  i_rAddr1,
	//input  [2:0]  i_rAddr2,
	output [15:0] o_rData1,
	//output [15:0] o_rData2,
	
	// Write connections.
	input  [15:0] i_wData,
	input  [2:0]  i_wAddr,
	input         i_wEn,
	
	// Stack pointer connection.
	output [15:0] o_reportSP,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * TODO- desc.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Register slot wires.
wire [15:0] r0D, r0Q;
wire        r0En;
wire [15:0] r1D, r1Q;
wire        r1En;
wire [15:0] r2D, r2Q;
wire        r2En;
wire [15:0] r3D, r3Q;
wire        r3En;
wire [15:0] r4D, r4Q;
wire        r4En;
wire [15:0] r5D, r5Q;
wire        r5En;
wire [15:0] r6D, r6Q;
wire        r6En;
wire [15:0] r7D, r7Q;
wire        r7En;

// Read mux nets.
wire [15:0] data1r0XX, data1r1XX;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Register slots- registers directly accessible by register file user.
DffSynchEn R0[15:0] (
	.D(r0D),
	.Q(r0Q),
	.S(r0En),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn R1[15:0] (
	.D(r1D),
	.Q(r1Q),
	.S(r1En),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn R2[15:0] (
	.D(r2D),
	.Q(r2Q),
	.S(r2En),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn R3[15:0] (
	.D(r3D),
	.Q(r3Q),
	.S(r3En),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn R4[15:0] (
	.D(r4D),
	.Q(r4Q),
	.S(r4En),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn R5[15:0] (
	.D(r5D),
	.Q(r5Q),
	.S(r5En),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn R6[15:0] (
	.D(r6D),
	.Q(r6Q),
	.S(r6En),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn R7[15:0] (
	.D(r7D),
	.Q(r7Q),
	.S(r7En),
	.clk(i_clk),
	.rstn(i_rstn)
);
	
////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Handle register slot inputs.
assign r0D  = i_wData;
assign r0En = i_wEn & ~i_wAddr[2] & ~i_wAddr[1] & ~i_wAddr[0]; // wr 000
assign r1D  = i_wData;
assign r1En = i_wEn & ~i_wAddr[2] & ~i_wAddr[1] &  i_wAddr[0]; // wr 001
assign r2D  = i_wData;
assign r2En = i_wEn & ~i_wAddr[2] &  i_wAddr[1] & ~i_wAddr[0]; // wr 010
assign r3D  = i_wData;
assign r3En = i_wEn & ~i_wAddr[2] &  i_wAddr[1] &  i_wAddr[0]; // wr 011
assign r4D  = i_wData;
assign r4En = i_wEn &  i_wAddr[2] & ~i_wAddr[1] & ~i_wAddr[0]; // wr 100
assign r5D  = i_wData;
assign r5En = i_wEn &  i_wAddr[2] & ~i_wAddr[1] &  i_wAddr[0]; // wr 101
assign r6D  = i_wData;
assign r6En = i_wEn &  i_wAddr[2] &  i_wAddr[1] & ~i_wAddr[0]; // wr 110
assign r7D  = i_wData;
assign r7En = i_wEn &  i_wAddr[2] &  i_wAddr[1] &  i_wAddr[0]; // wr 111

//------------------------------------------------------------------------------
// Drive frist read register.
Mux4 M0[15:0] (
	.C(r0Q),           // X00? Get r0 (000)
	.D(r1Q),           // X01? Get r1 (001)
	.E(r2Q),           // X10? Get r2 (010)
	.F(r3Q),           // X11? Get r3 (011)
	.S(i_rAddr1[1:0]),
	.Y(data1r0XX)
);
Mux4 M1[15:0] (
	.C(r4Q),           // X00? Get r4 (100)
	.D(r5Q),           // X01? Get r5 (101)
	.E(r6Q),           // X10? Get r6 (110)
	.F(r7Q),           // X11? Get r7 (111)
	.S(i_rAddr1[1:0]),
	.Y(data1r1XX)
);
Mux2 M2[15:0] (
	.A(data1r1XX),     // 1XX? Process lower bits for 1XX
	.B(data1r0XX),     // 0XX? Process lower bits for 0XX
	.S(i_rAddr1[2]),
	.Y(o_rData1)
);

//------------------------------------------------------------------------------
// Drive stack pointer report.
assign o_reportSP = r7Q; // R7 is a "stack pointer" for auto overflow detect

endmodule
