// Check missing a semicolon causes and error.

int main() {
	int a;
	int b;
	int c;
	
	a = 3;
	b = 19;
	
	// ERR: No semicolon.
	c = a + b
	
	return 1;
}