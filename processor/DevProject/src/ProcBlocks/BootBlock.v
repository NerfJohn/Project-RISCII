/*
 * BootBlock.v
 *
 * "Circuit to copy storage contents to memory prior upon hard reset"
 */
module BootBlock (
    // Memory bus connections (write only).
    output [15:0] o_memAddr,
    inout  [15:0] io_memData,
    output        o_memEn,
    
    // Storage bus connections (core clock used for SCK).
    output        o_storeEn,
    output        o_storeSDI,
    input         i_storeSDO,
    
    // Generated control signal (origin of "booting" state).
    output        o_nowBooted,
    
    // Common signals.
    input         i_clk,
    input         i_rstn
);

/*
 * Boot Block: Copies binary sections to memory for execution.
 *
 * Ensures faster-access memory is properly set up for core processor to run
 * the programmed application. Block has full control until done booting, though
 * is designed to have a finite number of steps (ie will always finish).
 *
 * This block assumes it has full control of both memory and storage buses based
 * on the value of "o_nowBooted" (intended as a global signal). This block also
 * effectively dictates proper binary organization (see below).
 *
 * In general, the binary is a series of 16-bit words (big endian) that indicate
 * word values at incremented addresses (word addressable). The .text and .data
 * sections begin (ie lowest address) at 0x0000 and 0x8000, respectively.
 *
 * == State Machine ==
 * state name | value | exits for                | desc.
 * -----------+-------+--------------------------+------
 * Reset      | 0x0   | reset completes          | general landing after reset
 * Read Cmd   | 0x1   | 8-bit command sent       | start storage reading
 * Read Addr  | 0x2   | 16-bit address sent      | read from beginning
 * Text Meta  | 0x3   | 16-bits read/saved       | indicator of .text length
 * Text Vals  | 0x4   | saved address written to | copy .text section
 * Data Meta  | 0x5   | 16-bits read/saved       | indicator of .data length
 * Data Vals  | 0x6   | saved address written to | copy .data section
 * Done       | 0x7   | nothing                  | finished copying/booting
 *
 * == Expected Binary Format ==
 * address   | field name          | desc.
 * ----------+---------------------+------
 * <lowest>  | .text final address | last raw .text address used, 1 word long
 * ---       | .text section       | .text section values, 1+ words long
 * ---       | .data final address | last raw .data address used, 1 word long
 * <highest> | .data section       | .data section values, 1+ words long
 *
 * Design Notes:
 * 1) In order to read storage, this block contain a chip specfic "read" command
 * 2) Past first few states, counter overflow used vs reset to track bit counts
 * 3) Minimum binary example: 0x0000 0xdead 0x8000 0xbeef
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Wires related to state machine logic.
wire [2:0] stateD,     stateQ;
wire       stateEn;
wire       stateEn00X, stateEn01X, stateEn0XX;
wire       stateEn10X, stateEn11X, stateEn1XX;
wire [2:0] nextStateA, nextStateY, nextStateCarry;

// Wires related to bit counting logic.
wire [3:0] bitD,  bitQ;
wire       bitRstn;
wire [3:0] bIncA, bIncY, bIncCarry;
wire       bCnt7, bCnt15, bCnt16;
wire       pulseD, pulseQ;

// Computed signals based on state machine/bit count.
wire setRdCmd, enStorage, zeroSDI;
wire saveAddr, enMemory;
wire bootDone;

// Wires related to "data" register.
wire [15:0] dataD, dataQ;
wire [7:0]  dataUpperByte;

// Wires related to "setpoint" address register.
wire [15:0] setAddrD, setAddrQ;
wire        setAddrEn;

// Wires related to "use for memory" address register.
wire [15:0] useAddrD,  useAddrQ;
wire        useAddrEn;
wire        addrUpperBit;
wire [15:0] nextAddrA, nextAddrY, nextAddrCarry;

// Computed signals based on address registers.
wire addrMatch;

// Wires related to tristate driving memory data.
wire [15:0] triA, triY;
wire        triEn;

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------
// State machine- series of incremental states required to set up hardware.
DffSynchEn STATE[2:0] (
    .D(stateD),
    .Q(stateQ),
    .S(stateEn),
    .clk(i_clk),
    .rstn(i_rstn)
);
Add1 NEXT_STATE[2:0] ( // TODO- using as incrementor- refactor?
    .A(nextStateA),
    .B(3'b000),
    .I({nextStateCarry[1:0], 1'b1}),
    .S(nextStateY),
    .O(nextStateCarry)
);

//------------------------------------------------------------------------------
// Register + Incrementor implmenting counter (for counting bit being shifted).
DffSynch BIT_CNT[3:0] (
    .D(bitD),
    .Q(bitQ),
    .clk(i_clk),
    .rstn(bitRstn)
);
Add1 BIT_INC[3:0] ( // TODO- using as incrementor, but also accessing carry
    .A(bIncA),
    .B(4'b0000),
    .I({bIncCarry[2:0], 1'b1}),
    .S(bIncY),
    .O(bIncCarry)
);
DffSynch WORD_PULSE ( // "Extension" of BIT_CNT, though explicitly for pulses
    .D(pulseD),
    .Q(pulseQ),
    .clk(i_clk),
    .rstn(bitRstn)
);

//------------------------------------------------------------------------------
// Data Register- captures data being shifted in/out of storage.
DffSynch DATA_REG[15:0] (
    .D(dataD),
    .Q(dataQ),
    .clk(i_clk),
    .rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Sepoint address- set with binary metadata and used to copy sized section.
DffSynchEn SET_ADDR[15:0] (
    .D(setAddrD),
    .Q(setAddrQ),
    .S(setAddrEn),
    .clk(i_clk),
    .rstn(i_rstn)
);

//------------------------------------------------------------------------------
// "Use" address- address written to in memory.
DffSynchEn USE_ADDR[15:0] (
    .D(useAddrD),
    .Q(useAddrQ),
    .S(useAddrEn),
    .clk(i_clk),
    .rstn(i_rstn)
);
Add1 ADDR_INC[15:0] ( // TODO- using as incrementor- refactor?
    .A(nextAddrA),
    .B(16'b0000000000000000),
    .I({nextAddrCarry[14:0], 1'b1}),
    .S(nextAddrY),
    .O(nextAddrCarry)
);

//------------------------------------------------------------------------------
// Tristate for memory bus data.
Tristate DATA_TRI[15:0] (
    .A(triA),
    .Y(triY),
    .S(triEn)
);

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// State machine- incremental states, but different conditions for moving.
assign nextStateA = stateQ;
assign stateD     = nextStateY;
assign stateEn00X = ~stateQ[0] | bCnt7;                // "Mux w/ 1"
assign stateEn01X = bCnt16;
Mux2 M0 (
    .A(bCnt16),
    .B(bCnt16 & addrMatch),
    .S(stateQ[0]),
    .Y(stateEn10X)
);
assign stateEn11X = ~stateQ[0] & (bCnt16 & addrMatch); // "Mux w/ 0"
Mux2 M1 (
    .A(stateEn01X),
    .B(stateEn00X),
    .S(stateQ[1]),
    .Y(stateEn0XX)
);
Mux2 M2 (
    .A(stateEn11X),
    .B(stateEn10X),
    .S(stateQ[1]),
    .Y(stateEn1XX)
);
Mux2 M3 (
    .A(stateEn1XX),
    .B(stateEn0XX),
    .S(stateQ[2]),
    .Y(stateEn)
);

//------------------------------------------------------------------------------
// Implement "bit counter" sub-unit for tracking bit shifts.
assign bIncA   = bitQ;
assign bitD    = bIncY;
assign bCnt7   = bIncCarry[2];      // at 7 if trying to carry to "8"s place
assign bCnt15  = bIncCarry[3];      // at 15 if trying to carry to "16"s place
assign pulseD  = bCnt15;
assign bCnt16  = pulseQ;
assign bitRstn = i_rstn & ~(stateEn & ~stateQ[2] & ~stateQ[1]);

//------------------------------------------------------------------------------
// Compute controls signals from state machine/bit count logic.
assign setRdCmd  = ~(|stateQ);
assign enStorage = |stateQ & ~(&stateQ);
assign zeroSDI   = stateQ[2] | stateQ[1];
assign saveAddr  = (stateQ[2] ^ stateQ[1]) & stateQ[0] & bCnt16;
assign enMemory  = stateQ[2] & ~stateQ[0] & bCnt16;
assign bootDone  = &stateQ;

//------------------------------------------------------------------------------
// Handle data register shifting + prepping read command.
Mux2 M4 [7:0] (
    .A(8'h03),        // NOTE! External chip specific "Read" Command
    .B(dataQ[14:7]),
    .S(setRdCmd),
    .Y(dataUpperByte)
);
assign dataD = {dataUpperByte, dataQ[6:0], i_storeSDO};

//------------------------------------------------------------------------------
// Grab shifted out binary metadata.
assign setAddrD  = dataQ;
assign setAddrEn = saveAddr;

//------------------------------------------------------------------------------
// Handle incrementing/resetting of address used for writing memory.
assign nextAddrA = useAddrQ;
Mux2 M5 (
    .A(stateQ[2]),      // set address for text/data section...
    .B(nextAddrY[15]),  // or incremented address...
    .S(saveAddr),       // resets when new setpoint is chosen
    .Y(addrUpperBit)
);
assign useAddrD  = {addrUpperBit, nextAddrY[14:0] & ~{15{saveAddr}}};
assign useAddrEn = enMemory | saveAddr;

//------------------------------------------------------------------------------
// Compare addresses to know when section has been fully read.
assign addrMatch = ~(|(setAddrQ ^ useAddrQ));

//------------------------------------------------------------------------------
// Limit memory bus driving to strictly when needed.
assign triA  = dataQ;    // Note: pass through signal
assign triEn = enMemory;

//------------------------------------------------------------------------------
// Set storage bus connections.
assign o_storeEn  = enStorage;
assign o_storeSDI = dataQ[15] & ~zeroSDI;

//------------------------------------------------------------------------------
// Set memory bus connections.
assign o_memAddr  = useAddrQ;
assign io_memData = triY;
assign o_memEn    = enMemory;

//------------------------------------------------------------------------------
// Set external control signal.
assign o_nowBooted = bootDone;

endmodule
