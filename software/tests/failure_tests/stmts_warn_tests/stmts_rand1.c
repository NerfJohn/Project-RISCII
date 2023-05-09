// Check statement warnings using a random(ish) test case.

int main() {
	int foo;
	
	foo = 9;
	
	if (foo & 0x1) {return 1;}
	if (foo & 0x2) {return 1;}
	if (foo & 0x4) {return 1;}
	if (foo & 0x8) {return 1;}
	
	// WARN: No guaranteed return.
}