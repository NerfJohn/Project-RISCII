/*
 * RegFile.v
 *
 * "Common register file- 8 words, 2 reads and 1 write per cycle"
 */
module RegFile (
	// Read addresses.
	input  [2:0]  i_rAddr1,
	input  [2:0]  i_rAddr2,
	
	// Write inputs/controls (synchronous).
	input  [15:0] i_wData,
	input  [2:0]  i_wAddr,
	input         i_wEn,
	
	// Result of reads (asynchronous).
	output [15:0] o_rData1,
	output [15:0] o_rData2,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * Register File- "color palette" of the core processor.
 *
 * Implements eight 16-bit registers, where 2 can be asynchronously read and 1
 * can be written synchronously at any moment. Bypass circuits are not included,
 * meaning a upcoming write to a register will not be passed to the read.
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Wires related to registers or register file.
wire [15:0] regAllD;
wire [15:0] reg0Q,  reg1Q,  reg2Q,  reg3Q;
wire        reg0En, reg1En, reg2En, reg3En;
wire [15:0] reg4Q,  reg5Q,  reg6Q,  reg7Q;
wire        reg4En, reg5En, reg6En, reg7En;

// Nets used to control writing of registers.
wire isWr00X, isWr01X, isWr10X, isWr11X;

// Nets used to read out selected registers.
wire [15:0] rData1Highs, rData1Lows;
wire [15:0] rData2Highs, rData2Lows;

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------
// Eight 16-bit registers- "memory cells" of the register file.
DffSynchEn REG0[15:0] (
	.D(regAllD),
	.Q(reg0Q),
	.S(reg0En),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn REG1[15:0] (
	.D(regAllD),
	.Q(reg1Q),
	.S(reg1En),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn REG2[15:0] (
	.D(regAllD),
	.Q(reg2Q),
	.S(reg2En),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn REG3[15:0] (
	.D(regAllD),
	.Q(reg3Q),
	.S(reg3En),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn REG4[15:0] (
	.D(regAllD),
	.Q(reg4Q),
	.S(reg4En),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn REG5[15:0] (
	.D(regAllD),
	.Q(reg5Q),
	.S(reg5En),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn REG6[15:0] (
	.D(regAllD),
	.Q(reg6Q),
	.S(reg6En),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn REG7[15:0] (
	.D(regAllD),
	.Q(reg7Q),
	.S(reg7En),
	.clk(i_clk),
	.rstn(i_rstn)
);

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Determine general "write nets" for registers.
assign isWr00X = i_wEn & ~i_wAddr[2] & ~i_wAddr[1];
assign isWr01X = i_wEn & ~i_wAddr[2] & i_wAddr[1];
assign isWr10X = i_wEn & i_wAddr[2] & ~i_wAddr[1];
assign isWr11X = i_wEn & i_wAddr[2] & i_wAddr[1];

//------------------------------------------------------------------------------
// Connect registers to write logic to allow for writing to the register file.
assign regAllD = i_wData;
assign reg0En  = isWr00X & ~i_wAddr[0];
assign reg1En  = isWr00X & i_wAddr[0];
assign reg2En  = isWr01X & ~i_wAddr[0];
assign reg3En  = isWr01X & i_wAddr[0];
assign reg4En  = isWr10X & ~i_wAddr[0];
assign reg5En  = isWr10X & i_wAddr[0];
assign reg6En  = isWr11X & ~i_wAddr[0];
assign reg7En  = isWr11X & i_wAddr[0];

//------------------------------------------------------------------------------
// Determine first read register value.
Mux4 M0[15:0] (
	.A(reg4Q),
	.B(reg5Q),
	.C(reg6Q),
	.D(reg7Q),
	.S(i_rAddr1[1:0]),
	.Y(rData1Highs)
);
Mux4 M1[15:0] (
	.A(reg0Q),
	.B(reg1Q),
	.C(reg2Q),
	.D(reg3Q),
	.S(i_rAddr1[1:0]),
	.Y(rData1Lows)
);
Mux2 M2[15:0] (
	.A(rData1Highs),
	.B(rData1Lows),
	.S(i_rAddr1[2]),
	.Y(o_rData1)
);

//------------------------------------------------------------------------------
// Determine second read register value.
Mux4 M3[15:0] (
	.A(reg4Q),
	.B(reg5Q),
	.C(reg6Q),
	.D(reg7Q),
	.S(i_rAddr2[1:0]),
	.Y(rData2Highs)
);
Mux4 M4[15:0] (
	.A(reg0Q),
	.B(reg1Q),
	.C(reg2Q),
	.D(reg3Q),
	.S(i_rAddr2[1:0]),
	.Y(rData2Lows)
);
Mux2 M5[15:0] (
	.A(rData2Highs),
	.B(rData2Lows),
	.S(i_rAddr2[2]),
	.Y(o_rData2)
);

endmodule
