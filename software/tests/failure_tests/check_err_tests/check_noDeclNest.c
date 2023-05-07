// Check non-declared variable (used in nested scope) causes an error.

int main() {
	int a;
	
	a = 1;
	
	if (a) {
		if (a + 1) {
			while (a < 3) {
				
				// ERR: Undeclared variable.
				foo = 9;
			}
		}
	}
	
	return 1;
}