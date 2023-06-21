// Check logical operators behave with expected precedence.
int main() {
	int result;
	
	// Precedence: <Highest> ~ [<< >>] & ^ | <Lowest>
	
	// 0x8 | ((0xf & ((0x7fff >> (~0x1)) << 1) ^ 0x7);
	result = 0x8 | 0xf & 0x7fff >> ~0x1 << 1 ^ 0x7;
	
	return result + -14;
}