// Check multiply defined names (within nested scope) cause an error.

int main() {
	int a;
	
	a = 1;
	
	if (a) {
		if (a + 1) {
			while (a < 3) {
				int foo;
				// ERR: Multiply defined name.
				char foo;
				
				foo = 9;
			}
		}
	}
	
	return 1;
}