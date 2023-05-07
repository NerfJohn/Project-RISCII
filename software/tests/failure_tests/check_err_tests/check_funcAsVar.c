// Check using function as variable causes an error.

int foo() {return 1;}

int main() {
	int a;
	
	a = 2;
	
	// ERR: foo is a function, NOT a variable.
	if (foo) {
		a = 4;
	}
	
	return 1;
}