// Check boolean operators respond correctly a "greater" operand.
int main() {
	int grt;
	int lt;
	int geq;
	int leq;
	int eq;
	int neq;
	int not;
	
	grt = 1 > 0;
	lt = 1 < 0;
	geq = 1 >= 0;
	leq = 1 <= 0;
	eq = 1 == 0;
	neq = 1 != 0;
	not = !1;
	
	// 1 + 0 + 1 + 0 + 0 + 1 + 0 = 3
	return grt + lt + geq + leq + eq + neq + not + -2;
}