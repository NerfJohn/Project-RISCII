// Checks behavior of minus vs. subtraction is understood.
int main() {
	int a;
	int b;
	int c;
	
	a = 24;
	b = 5;
	c = 100;
	
	int minusMinus;
	int subMinus;
	int minusSub;
	
	minusMinus = - -a;
	subMinus = b - -b;
	minusSub = -c - c;
	
	// 24 + 10 + -200 = -166
	return minusMinus + subMinus + minusSub + 167;
}