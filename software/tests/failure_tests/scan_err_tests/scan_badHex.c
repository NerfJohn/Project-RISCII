// Check a bad "hex" literal is considered an error.

int main() {
	int a;
	
	// ERR: 'G' cannot be used in a hex literal.
	a = 0x7G2;
	
	return 1;
}