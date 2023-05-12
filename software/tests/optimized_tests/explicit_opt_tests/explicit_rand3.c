// Check explicit optimizations (assuming -O1 flag) using random(ish) test case.

int result;

int main() {
	result = 0;
	
	int TRUE;
	TRUE = 1;
	
	// OPT: Should simplify to "result = 1;".
	if (TRUE) {
		if (!TRUE) {
			if (TRUE) {
				if (TRUE) {
					result = 0;
				}
			}
		}
		
		if (TRUE) {
			if (TRUE) {
				result = 1;
			}
		}
	}
	
	return result;
}