// Check using an unsigned int before initializing it causes a warning (error for -Werror).

int main() {
	unsigned int c;
	unsigned int ui;
	
	// WARN: unsigned int 'c' is uninitialized.
	ui = c;
	
	return 1;
}