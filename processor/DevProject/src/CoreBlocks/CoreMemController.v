/*
 * CoreMemController.v
 *
 * "Memory controller specific to core's instruction/data memory pipelines"
 */
module CoreMemController (
    // Instruction memory inputs (implied enable/read- also word addressable).
    input  [14:0] i_iAddr,
    
    // Unified memory interface (data for writing, implied enable).
    output [15:0] o_coreAddr,
    inout  [15:0] io_coreData,
    output        o_coreWr
);

/*
 * TODO- desc. module
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

// TODO- For now, just route instruction reads. Will implement data later.
assign o_coreAddr = {1'b0, i_iAddr}; // 2-byte access, instruction range
assign o_coreWr = 1'b0;

endmodule
