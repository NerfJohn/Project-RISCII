// Check const conditional for while block is implemented correctly (assuming -O1 flag).

int main() {
	int a;
	
	a = 1;
	
	// OPT: Delete entire block.
	while (0) {
		a = 0;
	}
	
	return a;
}