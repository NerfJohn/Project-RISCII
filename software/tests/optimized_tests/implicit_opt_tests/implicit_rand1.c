// Check implicit optimizations (assuming -O1 flag) using random(ish) test case.

int result;

int main() {
	result = 0;
	
	// OPT: Should simplify to "result = 1;".
	if (1) {
		if (0) {
			if (1) {
				if (1) {
					result = 0;
				}
			}
		}
		
		if ('F') {
			if (0x2) {
				result = 1;
			}
		}
	}
	
	return result;
}