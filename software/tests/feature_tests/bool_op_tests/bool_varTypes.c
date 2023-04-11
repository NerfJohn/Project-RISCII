// Check boolean operators allow for variable types.
int main() {
	int i;
	unsigned int ui;
	char c;
	unsigned char uc;
	
	i = 5;
	ui = 50;
	c = 0;
	uc = 0;
	
	int grt;
	int lt;
	int geq;
	int leq;
	int eq;
	int neq;
	int not;
	
	grt = ui > uc;
	lt = c < i;
	geq = c >= uc;
	leq = c <= ui;
	eq = uc == c;
	neq = i != ui;
	not = !uc;
	
	// Seven 1s.
	return grt + lt + geq + leq + eq + neq + not + -6;
}