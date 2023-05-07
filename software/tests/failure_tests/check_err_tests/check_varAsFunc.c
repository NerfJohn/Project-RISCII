// Check using a variable as a function causes and error.

int main() {
	int foo;
	int bar;
	
	// ERR: foo is a variable, NOT a function.
	bar = foo();
	
	return 1;
}