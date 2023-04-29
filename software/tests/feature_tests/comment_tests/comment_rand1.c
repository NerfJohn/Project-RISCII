// Check comments with random(ish) test case.

// (Pulled code from while_tests/while_rand2.c.)

// Main program: Counts the number of 1s in stream- confirming it to be 7.
int main() {
	unsigned int bitStream;	// Stream of bits
	int bitCnt;				// Counter of number of 1s in stream
	
	bitStream = 0xA50E;		// Preset value: 0xA50E (7 1s present).
	bitCnt = 0;
	
	// Keep counting until no 1s remain (also- soft check of uints and right shifting).
	while (bitStream != 0x0) {
		int i;
		i = 0;
		
		// Count 1 nibble (ie 4 bits) at a time.
		while(i < 4) {
			bitCnt = bitCnt + (bitStream & 0x1); 	// Count
			bitStream = bitStream >> 1;				// To next
			i = i + 1;								// Position in nibble
		}
	}
	
	// Confirm 7 1s were present in 0xA50E.
	return bitCnt == 7;
}