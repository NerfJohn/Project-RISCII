/*
 * ScanRegReporter.v
 *
 * "ScanReg with additional IO for reporting state- can still drive and chain"
 */
module ScanRegReporter (
    // Typical shift register IO.
    input  i_shiftIn,
    output o_shiftOut,
    
    // Line probed by scan register.
    inout  io_probeLine,
    
    // Control lines to enable settings/shifting.
    input  i_canDrive,
    input  i_doShift,
     
    // Report status of driving probe (for external use).
    output o_isDriving, 
    
    // Common signals.
    input  i_clk,
    input  i_rstn
);

/*
 * Scan Register Reporter- unit of Boundary Scan w/ exta signal
 *
 * Register implements a 3-bit control over a given probed line. Controls act
 * similar to GPIO controls, allowing for reading and writing of probed line.
 * Unit is designed to be part of a larger boundary scan register.
 *
 * Bit field controls can be updated via shifting, though doing so prevents
 * both driving/writing and fresh reading of the pin. Register must be given
 * permission to drive line (to mitigate contention).
 *
 * Unit also contains an extra output- reporting the status of the tristate
 * field driving the pin probe. Signal is intended to be used to probibit
 * related signals from being driven when this module is driving its probe.
 *
 * == Settings ==
 * bit field | name     | desc.
 * ----------+----------+------
 * [0]       | input    | read/sampled line, doesn't sample while shifting
 * [1]       | tristate | control for driving probe line (active HIGH)
 * [2]       | output   | value to drive probed line to when driving
 *
 * == Controls Summary ==
 * i_canDrive | i_doShift | can drive output? | is shifting fields?
 * -----------+-----------+-------------------+--------------------
 * 0          | 0         | no                | no
 * 0          | 1         | no                | yes
 * 1          | 0         | yes               | no
 * 1          | 1         | no                | yes
 * 
 * Design Notes:
 * 1) Assumes clock is JTAG's TCK (or other not core clk- uses asynh resets)
 * 2) Assumes driver of 'i_canDrive' is correct in its drive timing
 * 3) Fresh reading relies on 'i_clk' providing 2+ cycles for read
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Wires related to "sampling" DFF for reading.
wire sampleD, sampleQ;

// Wires related to core 3 bit fields.
wire outD, outQ, outEn;
wire triD, triQ, triEn;
wire inpD, inpQ;

// Computed signal relating to if register is driving probed line.
wire isDrivingProbe;

// Wires related to tristate driving probed line.
wire driveA, driveY, driveEn;

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------
// Sample DFF for reading- implements half of synchronized read signal.
DffAsynch SAMPLE_BIT (
    .D(sampleD),
    .Q(sampleQ),
    .clk(i_clk),
    .rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Core 3 fields of the register- output, enable, and input.
DffAsynchEn OUT_BIT (
    .D(outD),
    .Q(outQ),
    .S(outEn),
    .clk(i_clk),
    .rstn(i_rstn)
);
DffAsynchEn TRI_BIT (
    .D(triD),
    .Q(triQ),
    .S(triEn),
    .clk(i_clk),
    .rstn(i_rstn)
);
DffAsynch INP_BIT (
    .D(inpD),
    .Q(inpQ),
    .clk(i_clk),
    .rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Controlled tristate to drive probed line on command (if allowed to).
Tristate LINE_DRIVE (
    .A(driveA),
    .Y(driveY),
    .S(driveEn)
);

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Sample probed line for input bit field.
assign sampleD = io_probeLine;

//------------------------------------------------------------------------------
// Update input field based requested operation (read vs. shift).
Mux2 M0 (
    .A(i_shiftIn),
    .B(sampleQ),
    .S(i_doShift),
    .Y(inpD)
);

//------------------------------------------------------------------------------
// Handle shifting between bit fields.
assign triD  = inpQ;
assign triEn = i_doShift;
assign outD  = triQ;
assign outEn = i_doShift;

//------------------------------------------------------------------------------
// Compute signal controling if probe is driven (prevent "shift-to-on" event).
assign isDrivingProbe = triQ & i_canDrive & ~i_doShift;

//------------------------------------------------------------------------------
// Drive probed line as allowed and commanded to.
assign driveA  = outQ;
assign driveEn = isDrivingProbe;

//------------------------------------------------------------------------------
// Drive output of shifting register.
assign o_shiftOut = outQ;

//------------------------------------------------------------------------------
// Drive probed line (as able).
assign io_probeLine = driveY;

//------------------------------------------------------------------------------
// Report status of probe drive.
assign o_isDriving = isDrivingProbe;

endmodule
