/*
 * StorageController.v
 *
 * "'Mux' + logic deciding who controls lines to external storage"
 */
module StorageController (
    // Controls from memory-mapped peripheral.
    input i_mapEn,
    input i_mapSDI,
    
    // Controls from JTAG port.
    input i_jtagTCK,
    input i_jtagEn,
    input i_jtagSDI,
    
    // Controls from Boot circuit.
    input i_bootEn,
    input i_bootSDI,
    
    // Control signals deciding which circuit is in control.
    input i_isPaused,
    input i_isBooted,
    
    // Selected controls sent over SPI connection.
    inout io_storeSDI,
    inout io_storeSCK,
    inout io_storeSCS,
 
    // Control signal deciding when controller can drive memory bus.
    input i_disableDrive,
    
    // Common signals.
    input i_clk,
    input i_rstn
);

/*
 * Storage (via SPI) Controller:
 *
 * Routes storage bus signals of responsible circuit to the main storage
 * bus (based on MCU state signals). Also can stop driving lines to avoid
 * contention, as well as generate proper SCS vs SCK timing.
 *
 * Besides JTAG, the core clock is used for timing- bus signals should be
 * synched to the appropriate clock to run correctly. Enable should be
 * asserted 1 cycle before 1st shift and deasserted as last shift happens.
 *
 * state   | control circuit | desc.
 * --------+-----------------+------
 * Booting | Boot Block      | MCU is copying binary from external storage
 * Running | Mapped Port     | MCU may use port to access external storage
 * Paused  | JTAG Port       | MCU paused for probing via JTAG
 *
 * Design Notes:
 * 1) SCS active on "En" rising and in-active 1 SCK cycle after "En" falling
 * 2) SCK held low- can rise 1 SCK cycle after "En" rises (until "En" falls)
 * 3) Assumes input clocks and enables are stable and synchronous
 * 4) SCK may "blip" as SCS rises- *should* be small enough to ignore
 * 5) Assumes "i_isPaused" is updated in synch w/ "i_isBooted" (for storeClk)
 * 6) Mapped port is controllable, but not effective, in 'paused' state
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Nets related to muxing of storage control signals.
wire storeClk, storeEn, hasBootedEn;

// Nets related to muxing of storage data signal.
wire curSDI, hasBootedSDI;

// Wires related to "select" register (for timing of enable signal).
wire selD, selQ;

// Nets related to determining current value of control signals.
wire curSCS, curSCK;

// Wires related to tristate driving control signals to pins.
wire [2:0] triA, triY;
wire       triS;

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------
// "Select" register- used to ensure proper SCS vs SCK timing.
DffAsynch SELECT (
    .D(selD),
    .Q(selQ),
    .clk(storeClk),
    .rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Tristate controlling when storage controls can be applied/drive.
Tristate TRI[2:0] (
    .A(triA),
    .Y(triY),
    .S(triS)
);

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Determine storage signals to use for storage bus.
Mux2 M0 (
    .A(i_jtagTCK),
    .B(i_clk),
    .S(i_isPaused),
    .Y(storeClk)
);
Mux2 M1 (
    .A(i_jtagEn),
    .B(i_mapEn),
    .S(i_isPaused),
    .Y(hasBootedEn)
);
Mux2 M2 (
    .A(hasBootedEn),
    .B(i_bootEn),
    .S(i_isBooted),
    .Y(storeEn)
);

//------------------------------------------------------------------------------
// Determine data input to use for storage bus.
Mux2 M3 (
    .A(i_jtagSDI),
    .B(i_mapSDI),
    .S(i_isPaused),
    .Y(hasBootedSDI)
);
Mux2 M4 (
    .A(hasBootedSDI),
    .B(i_bootSDI),
    .S(i_isBooted),
    .Y(curSDI)
);

//------------------------------------------------------------------------------
// Set up "select" register for SCS vs SCK timing.
assign selD = storeEn;

//------------------------------------------------------------------------------
// Determine current control values to output.
assign curSCS = ~(storeEn | selQ);
assign curSCK = storeClk & selQ;

//------------------------------------------------------------------------------
// Use tristate to drive storage bus when allowed.
assign triA = {curSDI, curSCS, curSCK};
assign triS = ~i_disableDrive;

//------------------------------------------------------------------------------
// Drive outputs/storage control signals.
assign io_storeSDI = triY[2];
assign io_storeSCS = triY[1];
assign io_storeSCK = triY[0];

endmodule
