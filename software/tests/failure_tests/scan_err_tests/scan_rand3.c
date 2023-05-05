// Check scan phase errors using random(ish) test case.

int main () {
	unsigned int foobar;
	
	// ERR: "0x" prefix required for hex literal.
	foobar = 2Ab1;
	
	return 1;
}