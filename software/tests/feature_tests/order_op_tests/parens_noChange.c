// Check parentheses don't affect operation if ordering follows precedence.
int main() {
	
	int parens;
	int noParens;
	
	noParens = ~0xfff0 & 0x3 ^ 0x1 | 0x9;
	parens = ((((~(0xfff0)) & 0x3) ^ 0x1) | 0x9);
	
	return parens - noParens + 1;
}