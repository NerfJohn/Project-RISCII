// Check boolean operators with random(ish) test case.
int main() {
	char MIN;
	unsigned char MAX;
	
	MIN = -128;
	MAX = 255;
	
	int grt;
	int lt;
	int geq;
	int leq;
	int eq;
	int neq;
	int not;
	
	grt = MIN > MAX;
	lt = MIN < MAX;
	geq = MIN >= MAX;
	leq = MIN <= MAX;
	eq = MIN == MAX;
	neq = MIN != MAX;
	not = !MIN;
	
	// Negate all- then add one.
	return grt + !lt + geq + !leq + eq + !neq + not + !0x0;
}