// Check var/func warnings with random(ish) test case.

unsigned int xorState;

// Xorshift algorithm (based off George Marsaglia's work).
unsigned int xorshift() {
	unsigned int result;
	result = xorState;
	
	result = result ^ (result << 6);
	result = result ^ (result >> 8);
	result = result ^ (result << 2);
	
	xorState = result;
	return result;
}

// WARN: Unused function.
int xorshift_init() {
	xorState = 0xAB21;
}

int main() {
	int r1;
	int r2;
	int r3;
	
	xorState = 0xAB21;
	
	r1 = xorshift();
	r2 = xorshift();
	r3 = xorshift();
	
	return (r1 ^ r2 ^ r3) - 0x7726;
}