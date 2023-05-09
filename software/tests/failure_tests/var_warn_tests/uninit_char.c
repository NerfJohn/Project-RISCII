// Check using a char before initializing it causes a warning (error for -Werror).

int main() {
	char c;
	unsigned int ui;
	
	// WARN: char 'c' is uninitialized.
	ui = c;
	
	return 1;
}