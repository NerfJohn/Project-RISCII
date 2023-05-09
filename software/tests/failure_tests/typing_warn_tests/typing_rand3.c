// Check typing warnings using random(ish) test case.

int main() {
	unsigned int oof;
	
	oof = 0x1;
	
	// WARN: returning 'uint' for 'int' function.
	return oof;