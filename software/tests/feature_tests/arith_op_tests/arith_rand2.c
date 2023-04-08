// Check arithmetic operators with random(ish) test case.
int main () {
	unsigned int ui;
	ui = 9;
	
	int result;
	
	// (-19) - (34) - (-9) - (-32) - (0) - (-13) = 1
	result = -0x13 -34- -ui - -0x20 - '\0' - -13;
	
	// (- -(- -(-result))) = -result = -1
	result = - - - - -result;
	
	return result + 2;
}