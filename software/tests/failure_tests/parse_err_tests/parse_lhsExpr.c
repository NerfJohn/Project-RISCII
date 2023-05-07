// Check an rvalue expression on the lhs causes an error.

int main() {
	int a;
	int b;
	
	a = 2;
	b = 3;
	
	// ERR: rvalue on lhs.
	b + a = a;
	
	return 1;
}