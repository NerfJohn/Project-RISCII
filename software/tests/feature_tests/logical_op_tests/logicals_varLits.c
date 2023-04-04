// Check logical operators can use mixes of variable and literal operands.
int main() {
	// Stand-in variables for testing.
	int int_t;
	unsigned int uint_t;
	char char_t;
	unsigned char uchar_t;
	int_t = 14;
	uint_t = 3;
	char_t = 10;
	uchar_t = 7;
	
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
	and = 10000 & uint_t;
	or = uchar_t | 'g';
	xor = char_t ^ 0x45b;
	not = ~int_t;
	
	// Shift Notes:
	// Behavior undefined if 2nd operand doesn't fir in 4-bit unsigned value.
	// GCC seems to treat hex/char literals as naturally unsigned and int literals as naturally signed.
	shl0 = int_t << 14;
	shl1 = '\\' << uchar_t;
	shr0 = char_t >> 0x3;
	shr1 = -5 >> uint_t;
	
	// Return sum to check results (0 + 103 + 1105 + -15 + -32768 + 11776 + 1 + -1 = -19799).
	return and + or + xor + not + shl0 + shl1 + shr0 + shr1 + 19800;
}