// Check implicit optimizations (assuming -O1 flag) using random(ish) test case.

int longSpin() {
	int i;
	i = 0;
	while(i >= 0) {
		i = i + 1;
	}
	
	return 0;
}

int main() {
	// OPT: Delete entire block (and unused function).
	while (0x0) {
		int i;
		i = 0;
		while (i >= 0) {longSpin(); i = i + 1;}
	}
	
	return 1;
}