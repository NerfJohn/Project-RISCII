// Check boolean operators accept weird spacing.
int main() {
	int i;
	unsigned int ui;
	char c;
	unsigned char uc;
	
	i = 5;
	ui = 50;
	c = 1;
	uc = 2;
	
	int grt;
	int lt;
	int geq;
	int leq;
	int eq;
	int neq;
	int not;
	
	grt = 0x7777      >uc;
	lt = c 
	<  	 	 	'0';
	geq = c>=1;
	leq = 0x32
	<=
	ui;
	eq = uc 	 	 		== 	 	 	 	2;
	neq = c!=
	0xffff;
	not = !
				'\0';
	
	// Seven 1s.
	return grt + lt + geq + leq + eq + neq + not + -6;
}