// Check boolean operators allow for literal types.
int main() {
	int grt;
	int lt;
	int geq;
	int leq;
	int eq;
	int neq;
	int not;
	
	grt = 5 > 2;
	lt = 0x09 < 'z';
	geq = '\n' >= '\t';
	leq = 0x40 <= 64;
	eq = 65 == 'A';
	neq = 0x0 != 1;
	not = !0;
	
	// Seven 1s.
	return grt + lt + geq + leq + eq + neq + not + -6;
}