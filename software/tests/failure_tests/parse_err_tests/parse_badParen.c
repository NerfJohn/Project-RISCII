// Check bad parentheses causes an error.

int main() {
	int a;
	int b;
	
	a = 10;
	b = 45;
	
	// ERR: Bad parentheses.
	c = a (+ b);
	
	return 1;
}