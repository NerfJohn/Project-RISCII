// Check missing keyword causes an error.

// ERR: missing type keyword.
unsigned foo() {return 1;}

int main() {
	unsigned int a;
	
	a = foo();
	
	return 1;
}