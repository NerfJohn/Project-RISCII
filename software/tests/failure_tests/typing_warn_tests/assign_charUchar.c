// Check asigning a char to a unsigned char causes a warning (error for -Werror).

int main() {
	char a;
	unsigned char b;
	
	a = 'g';
	
	// WARN: assigned 'char' to 'uchar'.
	b = a;
	
	return 1;
}