// Check trying to call a function (after scoping made it a variable) causes an error.

int foo() {return 1;}

int main() {
	int foo;
	int bar;
	
	// ERR: foo refers to variable, NOT function.
	bar = foo();
	
	return 1;
}