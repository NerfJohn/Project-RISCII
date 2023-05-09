// Check overflowing int literal causes a warning (error for -Werror).

int main() {
	int foo;
	
	// WARN: 100,000 cannot fit in 16 bits.
	foo = 1000000;
	
	return 1;
}