// Check logical operators can use all variable types.
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
	and = int_t & uint_t;
	or = uchar_t | char_t;
	xor = char_t ^ int_t;
	not = ~uchar_t;
	
	// Shift Notes:
	// Behavior undefined if 2nd operand doesn't fir in 4-bit unsigned value.
	// GCC seems to treat hex/char literals as naturally unsigned and int literals as naturally signed.
	shl0 = int_t << int_t;
	shl1 = uchar_t << uint_t;
	shr0 = char_t >> char_t;
	shr1 = uint_t >> uint_t;
	
	// Return sum to check results (2 + 15 + 4 + -8 + -32768 + 56 + 0 + 0 = -32699).
	return and + or + xor + not + shl0 + shl1 + shr0 + shr1 + 32700;
}