// Check all operators behave expected precedence rules.
int main() {
	int result;
	result = 5;
	
	// Precedence (High to Low):
	// ~ -(unary) !
	// + -
	// << >>
	// < <= > >=
	// == !=
	// &
	// ^
	// |
	
	
	result = 0xfffc ^ 0 != 1 <= -2 < -result - ~0x2 + !result << 14 >> 15 >= 1 > -6 == 0 & 0xf | 0x3;
	// result = (-result) - (~0x2) + (!result); 	// -2
	// result = -result << 14 >> 15; 				// -1
	// result = 1 <= -2 < result >= 1 > -6;			// 1	
	// result = 0 != result == 0;					// 0
	// result = (0xfffc ^ (result & 0xf)) | 0x3;	// -1
	
	return -result;
}