// Check copy propagation for "blocks" are handled correctly (assuming -O1 flag).

int main() {
	int value;
	int result;
	
	value = 1;
	// result = 0; // Implicit to first call on stack
	
	if (value == 1) { // OPT: Inline block contents since "1 == 1"
		result = result | 0x1;
	}
	
	while (!value) { // OPT: Delete block since "!1 is 0"
		result = result | 0x2;
	}
	
	int constInt;
	constInt = 0x4;
	if (result != 1) {
		value = 0;
	}
	
	// OPT: result = 0x4 + value. (value no longer const after if, but constInt still is).
	result = constInt + value;
	
	return result == 5;
}