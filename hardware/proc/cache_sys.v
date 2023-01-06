// Cache System- manages transactions between processor, cache, and memory.
module cache_sys(pAddr, pData, pIsRd, pEn, doStall, pResult,
					mAddr, mData, mIsRd, mEn, done, mResult,
					clk, rstn);

// Std. clk and rstn.
input clk, rstn;

// I/Os with processor.
input pIsRd, pEn;
input[15:0] pAddr, pData;
output doStall;
output[15:0] pResult;

// I/Os with memory.
input done;
input[63:0] mResult;
output mIsRd, mEn;
output[15:0] mAddr;
output[63:0] mData;

// Intermediate wires (for setup/non state machine logic).
wire useMemBlock, useCacheAddr, outValid, outDirty, cacheHit, cacheEvict, tagMismatch;
wire[7:0] outTag;
wire[63:0] resultBlock, writeBlock, cacheBlock;

// Intermediate wires (for state machine logic).
wire[1:0] state;

// Cache instantiation.
cache iCACHE (.inIndex(pAddr[7:3]), .inBlock(writeBlock), .inTag(pAddr[15:8]), .inDirty(~pIsRd), .wr((pEn & cacheHit & ~pIsRd) | useMemBlock), 
				.outTag(outTag), .outBlock(cacheBlock), .outValid(outValid), .outDirty(outDirty), .clk(clk), .rstn(rstn));

/////////////////////////////////////////////////////////
// Setup Logic- State Machine Controlled/Used Elements //
/////////////////////////////////////////////////////////

// Summary computation of cache search (ie Is there a Hit? Will a miss require an evict?)
assign tagMismatch = (outTag[7] ^ pAddr[15]) | (outTag[6] ^ pAddr[14]) | (outTag[5] ^ pAddr[13]) | (outTag[4] ^ pAddr[12]) |
						(outTag[3] ^ pAddr[11]) | (outTag[2] ^ pAddr[10]) | (outTag[1] ^ pAddr[9]) | (outTag[0] ^ pAddr[8]);
assign cacheHit = outValid & ~tagMismatch;
assign cacheEvict = ~cacheHit & outDirty;

// Muxed input block to cache (choose between cache output and memory- then update word if applicable).
mux2 iMUXES0[63:0] (.A(mResult), .B(cacheBlock), .sel(useMemBlock), .out(resultBlock));
mux2 iMUXES1[15:0] (.A(pData), .B(resultBlock[15:0]), .sel(~pIsRd & (~pAddr[2] & ~pAddr[1])), .out(writeBlock[15:0]));
mux2 iMUXES2[15:0] (.A(pData), .B(resultBlock[31:16]), .sel(~pIsRd & (~pAddr[2] & pAddr[1])), .out(writeBlock[31:16]));
mux2 iMUXES3[15:0] (.A(pData), .B(resultBlock[47:32]), .sel(~pIsRd & (pAddr[2] & ~pAddr[1])), .out(writeBlock[47:32]));
mux2 iMUXES4[15:0] (.A(pData), .B(resultBlock[63:48]), .sel(~pIsRd & (pAddr[2] & pAddr[1])), .out(writeBlock[63:48]));

// Mux output of caching system (based on muxed "result block" from cache input).
mux4 iMUXES5[15:0] (.A(resultBlock[15:0]), .B(resultBlock[31:16]), .C(resultBlock[47:32]), .D(resultBlock[63:48]), .sel(pAddr[2:1]), .out(pResult));

// Setup connections to control memory (Mux requested address, set data source, and request type).
mux2 iMUXES6[15:0] (.A({outTag, pAddr[7:0]}), .B(pAddr), .sel(useCacheAddr), .out(mAddr));
assign mData = cacheBlock;
assign mIsRd = ~useCacheAddr;

///////////////////////////////////////////////
// Caching State Machine- Main Control Logic //
///////////////////////////////////////////////

// State- 00 = Idle, 01 = Read/Miss, 10 = Write/Evict.
dff_en iDFF0[1:0] (.D({~(^state) & cacheEvict, (~(^state) & ~cacheHit & ~cacheEvict) | state[1]}), 
					.Q(state), .en((~(^state) & pEn & ~cacheHit) | done), .clk(clk), .rstn(rstn));

// Assign control signals their values.
assign doStall = ~((~(^state) & (~pEn | cacheHit)) | (state[0] & done));
assign useMemBlock = state[0] & done;
assign useCacheAddr = ~(^state) & cacheEvict;
assign mEn = (~(^state) & pEn & ~cacheHit) | (state[1] & done);

endmodule