/*
 * BScanRegister.v
 *
 * "Circuit to parse JTAG signals into requests and control signals"
 */
module BScanRegister (
	// Typical shift register IO.
	input        i_shiftIn,
	output       o_shiftOut,
	
	// Memory connector probe lines.
	inout [15:0] io_memAddr,
	inout [15:0] io_memData,
	inout        io_memWr,
	inout        io_memEn,
	
	// Storage connector probe lines.
	inout        io_storeSCK,
	inout        io_storeSDI,
	inout        io_storeSDO,
	inout        io_storeSCS,
	
	// GPIO connector probe lines.
	inout [15:0] io_gpioPin,
	
	// Status connection probe lines.
	inout        io_isBooted,
	inout        io_isPaused,
	
	// Control lines to enable settings/shifting.
	input        i_canDrive,
	input        i_doShift,
	
	// Common signals.
	input        i_clk,
	input        i_rstn
);

/*
 * Boundary Scan Register- HW Pin Debugger
 *
 * Set of controls allowing user to read and write probed lines/pins as
 * permitted by the external controlling logic. Settings are shifted into
 * place, providing method of setting and reading the controls.
 *
 * While mostly a scaled up version of ScanReg.v, safety features are
 * present to prevent some lines (ie io_memData and io_storeSDI) from being
 * driven when their respectie enable is being driven (to prevent contention).
 *
 * TODO- update documentation blurb as module design is completed.
 *
 * == Settings (per probed line) ==
 * bit field | name     | desc.
 * ----------+----------+------
 * [0]       | input    | read/sampled line, doesn't sample while shifting
 * [1]       | tristate | control for driving probe line (active HIGH)
 * [2]       | output   | value to drive probed line to when driving
 *
 * == Probe Layout ==
 * probe position | name
 * ---------------+-----
 * [15:0]         | memory address    
 * [31:16]        | memory data       
 * [32]           | memory read/write 
 * [33]           | memory enable     
 * [34]           | storage SCK       
 * [35]           | storage SDI       
 * [36]           | storage SDO       
 * [37]           | storage SCS       
 * [53:38]        | GPIO              
 * [54]           | status isPaused   
 * [55]           | status isBooted   
 *
 * Design Notes:
 * 1) Extension of ScanReg.v and ScanRegReporter.v- see their notes
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Wires related to memory address scan register.
wire [15:0] memAddrIn, memAddrOut;

// Wires related to memory data scan register.
wire [15:0] memDataIn, memDataOut;
wire        memDataEn;

// Wires related to memory control registers.
wire memWrIn, memWrOut;
wire memEnIn, memEnOut, memEnDriving;

// Wires related to storage scan registers.
wire storeSCKIn, storeSCKOut;
wire storeSDIIn, storeSDIOut, storeSDIEn;
wire storeSDOIn, storeSDOOut;
wire storeSCSIn, storeSCSOut, storeSCSDriving;

// TODO- dummy wires to remove as rest of module is designed.
wire [17:0] dummyIn, dummyOut;

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------
// Boundary scan register related to memory address lines.
ScanReg MEM_ADDR[15:0] (
    // Typical shift register IO.
    .i_shiftIn(memAddrIn),
    .o_shiftOut(memAddrOut),
    
    // Line probed by scan register.
    .io_probeLine(io_memAddr),
    
    // Control lines to enable settings/shifting.
    .i_canDrive(i_canDrive),
    .i_doShift(i_doShift),
    
    // Common signals.
    .i_clk(i_clk),
    .i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Boundary scan register related to memory data lines.
ScanReg MEM_DATA[15:0] (
    // Typical shift register IO.
    .i_shiftIn(memDataIn),
    .o_shiftOut(memDataOut),
    
    // Line probed by scan register.
    .io_probeLine(io_memData),
    
    // Control lines to enable settings/shifting.
    .i_canDrive(memDataEn),
    .i_doShift(i_doShift),
    
    // Common signals.
    .i_clk(i_clk),
    .i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Boundary scan register related to memory control lines.
ScanReg MEM_WR (
    // Typical shift register IO.
    .i_shiftIn(memWrIn),
    .o_shiftOut(memWrOut),
    
    // Line probed by scan register.
    .io_probeLine(io_memWr),
    
    // Control lines to enable settings/shifting.
    .i_canDrive(i_canDrive),
    .i_doShift(i_doShift),
    
    // Common signals.
    .i_clk(i_clk),
    .i_rstn(i_rstn)
);
ScanRegReporter MEM_EN (
    // Typical shift register IO.
    .i_shiftIn(memEnIn),
    .o_shiftOut(memEnOut),
    
    // Line probed by scan register.
    .io_probeLine(io_memEn),
    
    // Control lines to enable settings/shifting.
    .i_canDrive(i_canDrive),
    .i_doShift(i_doShift),
	 
    // Report status of driving probe (for external use).
    .o_isDriving(memEnDriving),
    
    // Common signals.
    .i_clk(i_clk),
    .i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Boundary scan registers for storage data/control signals.
ScanReg STORE_SCK (
    // Typical shift register IO.
    .i_shiftIn(storeSCKIn),
    .o_shiftOut(storeSCKOut),
    
    // Line probed by scan register.
    .io_probeLine(io_storeSCK),
    
    // Control lines to enable settings/shifting.
    .i_canDrive(i_canDrive),
    .i_doShift(i_doShift),
    
    // Common signals.
    .i_clk(i_clk),
    .i_rstn(i_rstn)
);
ScanReg STORE_SDI (
    // Typical shift register IO.
    .i_shiftIn(storeSDIIn),
    .o_shiftOut(storeSDIOut),
    
    // Line probed by scan register.
    .io_probeLine(io_storeSDI),
    
    // Control lines to enable settings/shifting.
    .i_canDrive(storeSDIEn),
    .i_doShift(i_doShift),
    
    // Common signals.
    .i_clk(i_clk),
    .i_rstn(i_rstn)
);
ScanReg  STORE_SDO (
    // Typical shift register IO.
    .i_shiftIn(storeSDOIn),
    .o_shiftOut(storeSDOOut),
    
    // Line probed by scan register.
    .io_probeLine(io_storeSDO),
    
    // Control lines to enable settings/shifting.
    .i_canDrive(i_canDrive),
    .i_doShift(i_doShift),
    
    // Common signals.
    .i_clk(i_clk),
    .i_rstn(i_rstn)
);
ScanRegReporter STORE_SCS (
    // Typical shift register IO.
    .i_shiftIn(storeSCSIn),
    .o_shiftOut(storeSCSOut),
    
    // Line probed by scan register.
    .io_probeLine(io_storeSCS),
    
    // Control lines to enable settings/shifting.
    .i_canDrive(i_canDrive),
    .i_doShift(i_doShift),
	 
    // Report status of driving probe (for external use).
    .o_isDriving(storeSCSDriving),
    
    // Common signals.
    .i_clk(i_clk),
    .i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// TODO- dummy registers to be replaced with implemented scan registers.
DffAsynch DUMMY[17:0] (
	.D(dummyIn),
	.Q(dummyOut),
	.clk(i_clk),
	.rstn(i_rstn)
);

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Shift input through memory address' scan register (1st in shift line).
assign memAddrIn = {memAddrOut[14:0], i_shiftIn};

//------------------------------------------------------------------------------
// Shift input through memory data's scan register (2nd in shift line).
assign memDataIn = {memDataOut[14:0], memAddrOut[15]};
assign memDataEn = i_canDrive & ~memEnDriving;         // safety on ext. driving

//------------------------------------------------------------------------------
// Shift input through memory control scan registers (3rd/4th in shift line).
assign memWrIn = memDataOut[15];
assign memEnIn = memWrOut;

//------------------------------------------------------------------------------
// Shift input through storage scan registers (5th->8th in shift line).
assign storeSCKIn = memEnOut;
assign storeSDIIn = storeSCKOut;
assign storeSDIEn = i_canDrive & ~storeSCSDriving; // safety on ext. driving
assign storeSDOIn = storeSDIOut;
assign storeSCSIn = storeSDOOut;

//------------------------------------------------------------------------------
// TODO- dummy connections to replace with implemented scan registers.
assign dummyIn    = {dummyOut[16:0], storeSCSOut};
assign o_shiftOut = dummyOut[17];

endmodule
