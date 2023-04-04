// Check logical operators can use all literal types.
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
	and = 0x1 & 1;
	or = 'A' | 0x5555;
	xor = 255 ^ '\n';
	not = ~5;
	
	// Shift Notes:
	// Behavior undefined if 2nd operand doesn't fir in 4-bit unsigned value.
	// GCC seems to treat hex/char literals as naturally unsigned and int literals as naturally signed.
	shl0 = -1 << 1;
	shl1 = 0xab << 15;
	shr0 = 't' >> 0xa;
	shr1 = -2 >> '\n';
	
	// Return sum to check results (1 + 21845 + 245 + -6 + -2 + -32768 + 0 + -1 = -10686).
	return and + or + xor + not + shl0 + shl1 + shr0 + shr1 + 10687;
}