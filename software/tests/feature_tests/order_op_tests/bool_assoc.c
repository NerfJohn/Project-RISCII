// Check boolean operators behave with expected associativity.
int main() {
	int grt;
	int lt;
	int geq;
	int leq;
	int eq;
	int neq;
	int not;
	
	grt = 18 > 12 > 0;
	lt = 4 < 5 < 6;
	geq = 1 >= 2 >= 0;
	leq = 2 <= 5 <= 5;
	eq = 12 == 12 == 1;
	neq = 0 != 5 != 5;
	not = !!!0;
	
	// Seven 1s.
	return grt + lt + geq + leq + eq + neq + not + -6;
}