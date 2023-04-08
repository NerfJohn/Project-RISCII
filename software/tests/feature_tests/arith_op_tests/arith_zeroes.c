// Check arithmetic operators work with "0" literal.
int main() {
	// Vars to store results.
	int add;
	unsigned char sub;
	char minus;
	
	// Check operators.
	add = 5 + 0;
	sub = 6 - 0;
	minus = - 0;
	
	// Return summed result.
	return add + sub + minus - 10;
}