// Check boolean operators respond correctly to a "lesser" operand.
int main() {
	int grt;
	int lt;
	int geq;
	int leq;
	int eq;
	int neq;
	int not;
	
	grt = 0 > 1;
	lt = 0 < 1;
	geq = 0 >= 1;
	leq = 0 <= 1;
	eq = 0 == 1;
	neq = 0 != 1;
	not = !0;
	
	// 0 + 1 + 0 + 1 + 0 + 1 + 1 = 4
	return grt + lt + geq + leq + eq + neq + not + -3;
}