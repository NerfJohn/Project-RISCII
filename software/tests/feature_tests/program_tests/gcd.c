// Example program of a basic GCD algorithm.

// Rudimentary form is determining if modulo of the parameters is 0.
char isMod0(int dividend, int divisor) {
	int remainder;
	remainder = dividend;
	while (remainder > 0) {remainder = remainder - divisor;}
	return remainder == 0;
}

// Finds the greatest common denominator between parameters.
int gcd(int a, int b) {
	// First: establish smaller of the two arguments.
	int value;
	if (a > b) {value = b;}
	if (a <= b) {value = a;}
	
	// Iterate until options are exhaugsted.
	while (value > 0) {
		// Return GCD if found.
		if (isMod0(a,value) & isMod0(b,value)) {
			return value;
		}
		
		// Otherwise, check next lowest option.
		value = value - 1;
	}
}

int main() {
	// Inputs.
	int a;
	int b;
	
	a = 4;
	b = 6;
	
	int result;
	result = gcd(a,b);
	
	return result == 2;
}