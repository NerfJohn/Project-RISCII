// Check asigning an int to an unsigned int causes a warning (error for -Werror).

int main() {
	int a;
	unsigned int b;
	
	a = 0x30;
	
	// WARN: assigned 'int' to 'uint'.
	b = a;
	
	return 1;
}