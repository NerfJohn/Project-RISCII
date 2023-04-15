// Check parentheses do affect operation if ordering doesn't follow precedence.
int main() {
	
	int parens;
	int expected;
	
	parens = ~(0x8888 & (0x3 ^ (0x1 | 0x9)));
	expected = 0xfff7;
	
	return parens - expected + 1;
}