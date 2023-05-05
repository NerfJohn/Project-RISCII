// Check an "empty" hex literal is considered an error.

int main() {
	int a;
	
	// ERR: "0x" is not a valid hex literal.
	a = 0x;
	
	return 1;
}