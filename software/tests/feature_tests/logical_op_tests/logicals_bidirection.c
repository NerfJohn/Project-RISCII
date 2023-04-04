// Check bi-directional logical operators are, in fact, bidirectional.
int main() {
	// Var declarations for checking results.
	int and;
	int or;
	int xor;
	int temp;
	
	// Check each logical operator.
	temp = 0xdead & 0xbeef;
	and = 0xbeef & 0xdead;
	and = and + temp;
	
	temp = 0xdead | 0xbeef;
	or = 0xbeef | 0xdead;
	or = or + temp;
	
	temp = 0xdead ^ 0xbeef;
	xor = 0xbeef ^ 0xdead;
	xor = xor + temp;
	
	// Return sum to check results (0 + 0 + 0 = 0).
	return and + or + xor + 1;
}