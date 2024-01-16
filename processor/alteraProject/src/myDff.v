// D-Flip-Flop: Basis for all memory in processor/cache.
module myDff(D, Q, clk, rstn);

// clk/rstn signals.
input clk, rstn;

// I/Os.
input D;
output reg Q;

// Main logic: synch. update and asynch. reset.
always @(posedge clk, negedge rstn) begin
	if (~rstn)		Q <= 1'b0;	// Asynchronous, active-low reset
	else			Q <= D;		// Synchronous update
end

endmodule