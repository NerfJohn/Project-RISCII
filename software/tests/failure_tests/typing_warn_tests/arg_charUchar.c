// Check passing a char to a unsigned char causes a warning (error for -Werror).

unsigned char foo(unsigned char a) {return a;}

int main() {
	char a;
	
	a = 'g';
	
	// WARN: param 'uchar' given 'char'.
	foo(a);
	
	return 1;
}