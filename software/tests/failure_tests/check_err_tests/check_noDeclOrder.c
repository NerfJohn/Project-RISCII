// Check trying use a variable before its declaration causes an error.

int foo() {
	// ERR: global not declared yet.
	return global;
}

int global;

int main() {
	int a;
	
	global = 3;
	a = foo();
	
	return 1;
}