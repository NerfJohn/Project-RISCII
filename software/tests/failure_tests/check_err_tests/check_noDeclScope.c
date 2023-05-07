// Check trying use a variable out of its scope causes an error.

int main() {
	int a;
	
	a = 2;
	
	if (a) {
		int foo;
		
		foo = 2;
	}
	
	// ERR: Undeclared variable (in this scope).
	foo = 1;
	
	return 1;
}