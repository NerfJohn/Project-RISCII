/*
 * LazyPll.sv
 *
 * "Quick, custom PLL block to generate clock frequencies for development"
 */
module LazyPll (
    // Input 50 MHz clock.
    input  i_clock50,
    
    // Pulsing freq. selector.
    input  i_switchFreq,
    
    // Generated clock.
    output o_genClk
);

//////////////////////////////////////
// -- Signals/Internal Registers -- //
//////////////////////////////////////

// Internal counter (for clock ratio).
reg [23:0] clkCnt;

// Internal setting- 'pulse' input to switch output clock frequencies.
reg [23:0] clkThresh;

//////////////////////////////////
// -- Behavioral Block Logic -- //
//////////////////////////////////

//------------------------------------------------------------------------------
// Initial State- fresh counter, 'fast' clock starting LOW.
initial begin
    // Initialize PLL.
    clkCnt     = 24'h000000;
    clkThresh  = 24'h000006;  // 50 MHz / 6 = 8.333 MHz
    o_genClk   = 1'b0;
end

//------------------------------------------------------------------------------
// Input handling- update threshold register.
always @(posedge i_switchFreq) begin
    // Update threshold.
    if (clkThresh == 24'h000006) clkThresh = 24'h5F5E10; // 8.333 MHz -> 8 Hz
    else                         clkThresh = 24'h000006; // 8 Hz -> 8.333 MHz
end

//------------------------------------------------------------------------------
// Clock generation/switching.
always @(posedge i_clock50) begin
    // Increment internal counter.
    clkCnt = clkCnt + 1;
    
    // Toggle output as needed.
    if (clkCnt >= clkThresh) begin
        o_genClk = ~o_genClk;     // Toggle generated clock
        clkCnt   = 0;             // Reset counter/ clock ratio
    end
end

endmodule
 