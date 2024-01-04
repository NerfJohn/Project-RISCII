module LazyPll(inClk, outClk);

// Parameter to control division done by "PLL".
parameter DIVIDER = 1; // Default to no division.

// Inputs/Outputs.
input  inClk;  // Input clock signal
output outClk; // Produced output clock

// Internal/Intermediate signals.
reg[31:0] divCnt;

// Intialize controlled signals.
initial begin
     // Internal signals.
     divCnt = 0;

    // Output signals.
     outClk = 0;
end

// Bookkeep and update clock as needed.
always @(posedge inClk) begin
    // Increment internal counter.
    divCnt = divCnt + 1;
    
    // Toggle outputs as applicable.
    if (divCnt == DIVIDER) begin
        outClk = ~outClk;
        divCnt = 0;          // Reset internal counter as well
    end
end

endmodule
