// Check logical operators behave as expected for "mixed bits" operands.
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
	and = 0x1 & 0x5555;
	or = 0x1 | 0x5555;
	xor = 0x1 ^ 0x5555;
	not = ~0x5555;
	
	shl0 = 0x1 << 0x5;
	shl1 = 0x5555 << 0x1;
	shr0 = 0x1 >> 0x5;
	shr1 = 0x5555 >> 0x1;
	
	// Return sum to check results (1 + 21845 + 21844 + -21846 + 32 + -21846 + 0 + 10922 = 10952).
	return and + or + xor + not + shl0 + shl1 + shr0 + shr1 + -10951;
}