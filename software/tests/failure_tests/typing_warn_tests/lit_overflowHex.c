// Check overflowing hex literal causes a warning (error for -Werror).

int main() {
	int foo;
	
	// WARN: 0x01234567 cannot fit in 16 bits.
	foo = 0x01234567;
	
	return 1;
}