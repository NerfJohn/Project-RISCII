module sim_top();

// Parameter- specifies if simulation is a test/should include halt tests.
parameter IS_TEST = 0; // Not test by default

// Signals to connect to DUT.
logic pwrOn, clk, halt;
logic [15:0] firstWord;

// "System-on-Chip" design with DUT.
dut_soc iDUT(.pwrOn(pwrOn), .clk(clk), .firstWord(firstWord), .halt(halt));

// Clk counter for testing/output information.
integer clkCnt;

// "Snapshot" values when halt is detected.
logic haltDetected;
logic [15:0] wordAtHalt;
integer clkAtHalt;

// File descriptor for output file.
integer outFile;

// Load/Startup script.
initial begin
	// Initialize values.
	clkCnt = 0;
	haltDetected = 0;
	wordAtHalt = 0;
	clkAtHalt = 0;
	outFile = 0;
	
	// Power on DUT.
	pwrOn = 1;
end

// Main loop for testing/checking.
always @(posedge clk) begin
	// Delay to ensure all signals are resolved after clk (including memory at #1).
	#2;
	
	// Increment clk counter.
	clkCnt = clkCnt + 1;
	
	// Capture details of first halt.
	if ((haltDetected == 0) && (halt)) begin
		haltDetected = 1;
		wordAtHalt = firstWord;
		clkAtHalt = clkCnt;
	end
	
	//$display("r6=%d (0x%x)", iDUT.dut.iFILE.iREGS0[6].Q[15:0], iDUT.dut.iFILE.iREGS0[6].Q[15:0]);
	//$display("sp=%x, fp=%x", iDUT.dut.iFILE.iREGS0[4].Q[15:0], iDUT.dut.iFILE.iREGS0[5].Q[15:0]);
	
	// Run additional tests if requested (all exit before file creation).
	if (IS_TEST) begin
		// Check halt is asserted in time.
		if ((haltDetected == 0) && (clkCnt > 3000)) begin
			$display("ERR: Halt not asserted in time!");
			$finish;
		end
		
		// Check halt stays asserted.
		if (haltDetected && (halt == 0)) begin
			$display("ERR: Halt de-asserted!");
			$finish;
		end
		
		// Check firstWord stays consistent on halt.
		if (haltDetected && (wordAtHalt != firstWord)) begin
			$display("ERR: First word changed while halted!");
			$finish;
		end
	end
	
	// Successfully exit when halt condition(s) reached.
	if (haltDetected && ((clkCnt - clkAtHalt) == 5)) begin
		// Write the output file.
		outFile = $fopen("sim_output");
		$fdisplay(outFile, "%d\n%d", wordAtHalt, clkCnt);
		$fclose(outFile);
		
		// Exit the simulation (successfully- file created).
		$finish;
	end
end

endmodule