// Check logical operators behave as expected for "all ones" operands.
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
	and = 0x1 & 0xffff;
	or = 0x1 | 0xffff;
	xor = 0x1 ^ 0xffff;
	not = ~0xffff;
	
	// Shift Notes:
	// Behavior undefined if 2nd operand doesn't fir in 4-bit unsigned value.
	// GCC seems to treat hex/char literals as naturally unsigned and int literals as naturally signed.
	shl0 = 0x1 << 0xf;
	shl1 = 0xffff << 0x1;
	shr0 = 0x1 >> 0xf;
	shr1 = 0xffff >> 0x1;
	
	// Return sum to check results (1 + -1 + -2 + 0 + -32768 + -2 + 0 + 32767 = -4).
	return and + or + xor + not + shl0 + shl1 + shr0 + shr1 + 5;
}