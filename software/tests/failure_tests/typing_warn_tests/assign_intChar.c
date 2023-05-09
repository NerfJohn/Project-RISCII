// Check asigning an int to a char causes a warning (error for -Werror).

int main() {
	int a;
	unsigned char b;
	
	a = 4;
	
	// WARN: assigned 'int' to 'uchar'.
	b = a;
	
	return 1;
}