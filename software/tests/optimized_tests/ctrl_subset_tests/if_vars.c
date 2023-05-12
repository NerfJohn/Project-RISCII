// Check if statements create new scopes for variable names.
int main() {
	// "Foo" used in multiple scopes.
	int foo;
	int err;
	int bar;
	
	foo = 1;
	err = 0;
	bar = -100;
	
	if (1) {
		int foo;
		foo = 0;
	}
	
	if (1) {
		int foo;
		foo = 3;
		if (1) {
			int foo;
			foo = 0;
		}
		
		// Checks inner 'if' didn't affect outer 'if'.
		if (foo != 3) {
			err = 1;
		}
	}
	
	// Ensure new scoping takes effect AFTER declaration.
	if (1) {
		bar = 0;
		int bar;
		bar = -100;
	}
	
	// Checks 'if' scopes didn't affect outmost foo (but did affect bar).
	return foo + err + bar;
}