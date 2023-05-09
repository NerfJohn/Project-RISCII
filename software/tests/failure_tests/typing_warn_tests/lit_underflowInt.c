// Check underflowing int literal causes a warning (error for -Werror).

int main() {
	int foo;
	
	// WARN: -32769 cannot fit in 16 bits.
	foo = -32769;
	
	return 1;
}