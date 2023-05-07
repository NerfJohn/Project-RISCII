// Check too few arguments causes an error.

int foo(int p1, int p2) {return p1 + p2;}

int main() {
	int a;
	
	a = 3;
	
	// ERR: too few args.
	foo(a);
	
	return 1;
}