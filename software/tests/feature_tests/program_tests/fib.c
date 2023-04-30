// Example program to calculate the nth Fibonacci number.

unsigned int fib(unsigned char n) {
	// Handle known/starting cases.
	if (n <= 1) {return n;}
	
	// Recursive calculation.
	return fib(n - 1) + fib(n - 2);
}

int main() {
	return fib(6) - 7;
}