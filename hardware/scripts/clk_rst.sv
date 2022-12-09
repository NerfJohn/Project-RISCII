module clk_rst (pwrOn, clk, rstn);

// Inputs.
input pwrOn;

// Outputs.
output clk, rstn;

// "Middle-men" to appease verilog (outs can't be in behavioral block).
logic clkLogic, rstnLogic;

// "State" variable to keep track of reset cycles.
integer rstnActive;

// Handle power on sequence.
always @(posedge pwrOn) begin
	// Start on down cycle.
	clkLogic = 0;
	
	// Assert active-low reset for 5 positive edges of clk.
	rstnLogic = 0;
	rstnActive = 5;
end

// Clk Controller.
always begin
	// Clk Delay;
	#10;
	
	// Handle clk (if powered).
	if (pwrOn) clkLogic = ~clkLogic;
end

// Rstn Controller.
always @(posedge clk) begin
	// Turn off reset as needed.
	if (rstnActive > 0) rstnActive = rstnActive - 1;
	if (rstnActive == 0) rstnLogic = 1;
end


// Connect "middle-men" and outputs.
assign clk = clkLogic;
assign rstn = rstnLogic;

endmodule