// Check missing ID causes an error.

int main() {
	int a;
	int b;
	int c;
	
	a = 3;
	b = 4;
	
	// ERR: missing id between operators.
	c = a + ;
	
	return 1;
}