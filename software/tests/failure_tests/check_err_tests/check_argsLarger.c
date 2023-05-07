// Check too many arguments causes an error.

int foo(int p1) {return p1;}

int main() {
	int a;
	
	a = 3;
	
	// ERR: too many args.
	foo(a,a);
	
	return 1;
}