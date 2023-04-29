// Check while statements create new scopes for variable names.
int main() {
	// "Foo" used in multiple scopes.
	int foo;
	int err;
	int bar;
	int flag;
	
	foo = 1;
	err = 0;
	bar = 100;
	flag = 1;
	
	while (flag) {
		int foo;
		foo = 0;
		
		flag = !flag;
	}
	flag = !flag;
	
	while(flag) {
		int foo;
		foo = 2;
		
		while (flag) {
			int foo;
			foo = 0;
			flag = !flag;
		}
		
		// Check inner "while" didn't affect outer "while".
		if (foo != 2) {
			err = 1;
		}
	}
	flag = !flag;
	
	// Ensure new scoping takes effect AFTER declaration.
	while (flag) {
		bar = 0;
		int bar;
		bar = 100;
		flag = !flag;
	}
	
	// Checks 'while' scopes didn't affect outmost foo (but did affect bar).
	return foo + err + bar;
}