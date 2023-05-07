// Check missing operator causes an error.

int main() {
	int a;
	int b;
	int c;
	
	a = 1;
	b = 3;
	
	// ERR: missing operator.
	c = a b;
	
	return 1;
}