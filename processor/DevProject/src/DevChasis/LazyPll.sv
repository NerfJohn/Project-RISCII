/*
 * LazyPll.sv
 *
 * "Quick, custom PLL block to generate desired clock frequency for development"
 */
module LazyPll (
    // Input clock signal (expecting 50 MHz).
    input  pll_clock50_i,
    
    // Input for switch clock frequencies.
    input  pll_switchFreq_i,
    
    // Generated clock signal.
    output pll_genClk_o
);

////////////////////////
// -- Signals/Regs -- //
////////////////////////

// Internal counter tracking ratio of clock frequencies.
reg [23:0] clkCnt;

// Internal setting- 'pulse' input to switch output clock frequencies.
reg [23:0] clkThresh;

////////////////////////
// -- Blocks/Logic -- //
////////////////////////

// Initial State- fresh counter, 'fast' clock starting LOW.
initial begin
    // Initialize PLL.
    clkCnt       = 24'h000000;
    clkThresh    = 24'h000006;  // 50 MHz / 6 = 8.333 MHz
    pll_genClk_o = 1'b0;
end

// Input handling- update threshold register.
always @(posedge pll_switchFreq_i) begin
    // Update threshold.
    if (clkThresh == 24'h000006) clkThresh = 24'h5F5E10; // 8.333 MHz -> 8 Hz
    else                         clkThresh = 24'h000006; // 8 Hz -> 8.333 MHz
end

// Clock generation/switching.
always @(posedge pll_clock50_i) begin
    // Increment internal counter.
    clkCnt = clkCnt + 1;
    
    // Toggle output as needed.
    if (clkCnt >= clkThresh) begin
        pll_genClk_o = ~pll_genClk_o; // Toggle generated clock
        clkCnt = 0;                   // Reset counter/ clock ratio
    end
end

endmodule
 