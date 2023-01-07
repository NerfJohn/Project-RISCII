module mem_64k (inData, inAddr, isRd, startReq, outData, reqFinish, clk, rstn);

// Std. clk and reset signals.
input clk, rstn;

// Inputs.
input isRd, startReq;
input [15:0] inAddr;
input [63:0] inData;

// Outputs.
output reqFinish;
output [63:0] outData;

// Parameters.
parameter NON_VOLATILE = 0; // RAM vs. ROM- RAM = 0

// 64 KB array acting as core memory unit.
reg [7:0] coreMem[0:65535];

// "State" variables- cycles till result w/ request data.
integer curState;
logic reqWasRd;
logic [15:0] reqAddr;

// "Middle-men" to appease verilog (outs can't be in behavioral block).
logic reqFinishLogic;
logic [63:0] outDataLogic;

integer i; // Typical indexer

// Handle memory reset (for simplicity, handle ROM setting here, too).
always @(negedge rstn) begin
	// Zero out memory.
	for(i = 0; i < 65536; i++)
		coreMem[i] = 0;
	
	// Setup ROM code if applicable.
	if (NON_VOLATILE == 1)
		$readmemh("sim_input", coreMem);
	
	// Reset- perform "write" request.
	curState = 4;
	reqWasRd = 0;
	reqAddr = 0;
	outDataLogic = 0;
	reqFinishLogic = 0;
end

// Main logical loop.
always @(posedge clk) begin
	// Ensure all signals are resolved before evaluating.
	#1;

	// Decrement "cycle counter".
	if (curState >= 0)
		curState = curState - 1;
	
	// De-assert outputs as applicable.
	if (curState != 0)
		reqFinishLogic = 0;
		outDataLogic = 0;
	
	// Finish request when appropriate.
	if (curState == 0) begin
		// Set output data.
		for(i = 0; i < 4; i++)
			outDataLogic = {coreMem[reqAddr+(2*i)],coreMem[reqAddr+(2*i)+1],outDataLogic[63:16]};
		if (reqWasRd == 0) outDataLogic = 0;
		
		// Set flag signal.
		reqFinishLogic = 1;
	end
	
	// Handle read when able to.
	if ((curState <= 0) && (startReq)) begin
		// Record request data.
		reqWasRd = isRd;
		reqAddr = {inAddr[15:3], 3'b000};
		
		// Set curState.
		curState = (reqWasRd) ? 2 : 4;
		
		// Serve write (won't be seen anyway).
		if (reqWasRd == 0) begin
			coreMem[reqAddr+1] = inData[7:0];
			coreMem[reqAddr+0] = inData[15:8];
			coreMem[reqAddr+3] = inData[23:16];
			coreMem[reqAddr+2] = inData[31:24];
			coreMem[reqAddr+5] = inData[39:32];
			coreMem[reqAddr+4] = inData[47:40];
			coreMem[reqAddr+7] = inData[55:48];
			coreMem[reqAddr+6] = inData[63:56];
		end
	end
end

// Quick Fix: Retry request start for propogated signal.
always @(posedge startReq) begin
	// Handle read when able to.
	if ((curState == 0) && (startReq)) begin
		// Record request data.
		reqWasRd = isRd;
		reqAddr = {inAddr[15:3], 3'b000};
		
		// Set curState.
		curState = (reqWasRd) ? 2 : 4;
		
		// Serve write (won't be seen anyway).
		if (reqWasRd == 0) begin
			coreMem[reqAddr+1] = inData[7:0];
			coreMem[reqAddr+0] = inData[15:8];
			coreMem[reqAddr+3] = inData[23:16];
			coreMem[reqAddr+2] = inData[31:24];
			coreMem[reqAddr+5] = inData[39:32];
			coreMem[reqAddr+4] = inData[47:40];
			coreMem[reqAddr+7] = inData[55:48];
			coreMem[reqAddr+6] = inData[63:56];
		end
	end
end

// Connect the "middle-men" to their signals.
assign reqFinish = reqFinishLogic;
assign outData = outDataLogic;

endmodule