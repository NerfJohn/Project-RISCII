/*
 * StorageController.v
 *
 * "'Mux' + logic deciding who controls SPI lines to external storage"
 */
module StorageController (
    // Controls from memory-mapped peripheral.
    input mapEn,
    
    // Controls from JTAG port.
    input jtagTCK,
    input jtagEn,
    
    // Controls from Boot circuit.
    input bootEn,
    
    // Control signals deciding which circuit is in control.
    input isPaused,
    input isBooted,
    
    // Selected controls sent over SPI connection.
    output spiSCK,
    output spiSCS,
    
    // Common signals.
    input clk,
    input rstn
);

/*
 * Storage (via SPI) Controller:
 *
 * Mux/logic controlling who controls the SPI connection to external storage
 * (where control is effectively based on MCU's top-level state). Below
 * describes when each circuit has control.
 *
 * state   | control circuit | desc.
 * --------+-----------------+------
 * Booting | Boot Block      | MCU is copying binary from external storage
 * Running | Mapped SPI Port | MCU may use Mapped SPI to access external storage
 * Paused  | JTAG Port       | MCU paused for probing via JTAG
 *
 * Design Notes:
 * 1) SCS active on "En" rising and in-active 1 SCK cycle after "En" falling
 * 2) SCK held low- can rise 1 SCK cycle after "En" rises (until "En" falls)
 * 3) Assumes input clocks and enables are stable and synchronous
 * 4) SCK may "blip" as SCS rises- *should* be small enough to ignore
 */

/////////////////////////
// -- Signals/Wires -- //
/////////////////////////

// Select timing register wires.
wire selD, selQ;

// Computed clock source wires.
wire inJTAGMode, curClk;

// Computed "enable" wires.
wire hasBootedEn, curEn;

////////////////////////////
// -- Blocks/Instances -- //
////////////////////////////

// "Select" register- used to ensure proper SCS vs SCK timing.
DffAsynch SELECT (
    .D(selD),
    .Q(selQ),
    .clk(curClk),
    .rstn(rstn)
);

//////////////////////////
// -- Connects/Logic -- //
//////////////////////////

// Determine which clock to use.
assign inJTAGMode = isPaused & isBooted;
Mux2 M0 (
    .A(jtagTCK),
    .B(clk),
    .S(inJTAGMode),
    .Y(curClk)
);

// Determine which circuit should be in control.
Mux2 M1 (
    .A(jtagEn),
    .B(mapEn),
    .S(isPaused),
    .Y(hasBootedEn)
);
Mux2 M2 (
    .A(hasBootedEn),
    .B(bootEn),
    .S(isBooted),
    .Y(curEn)
);

// Control SCS and SCK for selected controls.
assign selD   = curEn;
assign spiSCS = ~(curEn | selQ);
assign spiSCK = curClk & selQ;

endmodule
