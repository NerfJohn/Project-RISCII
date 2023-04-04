// Check logical operators behave as expected for "all zeroes" operands.
int main() {
	// Var declarations for checking results.
	int and;
	int or;
	int xor;
	int not;
	int shl0;
	int shl1;
	int shr0;
	int shr1;
	
	// Check each logical operator.
	and = 0x1 & 0x0;
	or = 0x1 | 0x0;
	xor = 0x1 ^ 0x0;
	not = ~0x0;
	shl0 = 0x1 << 0x0;
	shl1 = 0x0 << 0x1;
	shr0 = 0x1 >> 0x0;
	shr1 = 0x0 >> 0x1;
	
	// Return sum to check results (0 + 1 + 1 + -1 + 1 + 0 + 1 + 0 = 3).
	return and + or + xor + not + shl0 + shl1 + shr0 + shr1 + -2;
}