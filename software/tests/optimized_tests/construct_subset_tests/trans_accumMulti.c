// Check compiler can successfully translate "multi-accumulator value" situations.
int main() {
	int a;
	int b;
	int c;
	int d;
	
	a = 1;
	b = 2;
	c = 3;
	d = 4;
	
	int res;
	
	// Requires at least 2 accumulator values to be known at once to compute.
	res = (a + b) + (c + d) + (a + b + c + d);
	
	return res - 19;
}