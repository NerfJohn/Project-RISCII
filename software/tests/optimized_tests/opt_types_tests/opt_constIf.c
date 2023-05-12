// Check const conditional for if block is implemented correctly (assuming -O1 flag).

int main() {
	int a;
	
	a = 0;
	
	// OPT: Remove conditional, inline statements.
	if (1) {
		a = 1;
	}
	
	return a;
}