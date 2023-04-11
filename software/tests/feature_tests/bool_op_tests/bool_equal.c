// Check boolean operators respond correctly to "equal operands".
int main() {
	int grt;
	int lt;
	int geq;
	int leq;
	int eq;
	int neq;
	int not;
	
	grt = 1 > 1;
	lt = 1 < 1;
	geq = 1 >= 1;
	leq = 1 <= 1;
	eq = 1 == 1;
	neq = 1 != 1;
	not = !1;
	
	// 0 + 0 + 1 + 1 + 1 + 0 + 0 = 3
	return grt + lt + geq + leq + eq + neq + not + -2;
}