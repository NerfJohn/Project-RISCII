// Check compiler can handle global accesses (when FP and SP are out of range).

// Stack starts at 0xffff, while .data section is around 0x0 in RAM.
// To ensure to "wrap around" accessing, attempt access after "x" calls deep.
// Load/store instructions have about 15 slots of accessing in each direction.

int global;

int foo(int doCall) {
	// Vars to bloat stack frames.
	int v1;
	int v2;
	int v3;
	int v4;
	int v5;
	int v6;
	
	if (doCall) {foo(doCall - 1);}
	if (!doCall) {global = 1;} // Access global variable once range is achieved
	
	return 0;
}

int main() {
	// 3 * about 6 = 18 slots; out of direct access range of globals.
	foo(3);
	
	return global;
}