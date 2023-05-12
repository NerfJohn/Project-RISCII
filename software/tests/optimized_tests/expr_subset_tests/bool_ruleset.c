// Check boolean operators follow "operand type ruleset".

// Note: The following ruleset outlines how types are determined:
// 1) char/uchar become 'int' type BEFORE being used.
// 2) int becomes 'uint' type if other operand is a uint.
// 3) all literals become 'int' type unless rule #4 applies.
// 4) int/hex literals with int over/underflow become 'uint' types.

// (Likely different from C/GCC, but similar enough for core project use.)
// (Being checked here as rules don't really affect logical/arith ops.)

int main() {
	char c;
	unsigned char uc;
	int i;
	unsigned int ui;
	
	c = 0xff;
	uc = 0xff;
	i = -1;
	ui = 1;
	
	int neq;
	int grt;
	int geq;
	int lt;
	
	neq = c != uc; // Rule 1: 0xffff != 0x00ff, NOT 0xff != 0xff
	grt = i > ui; // Rule 2: 65535 > 1, NOT -1 > 1
	geq = 1 >= 0x8000; // Rule 3: 1 >= -32768, NOT 1 >= 32768
	lt = 1 < -32769; // Rule 4: 1 < 32767, NOT 1 < -32769
	
	return neq + grt + geq + lt + -3;
}