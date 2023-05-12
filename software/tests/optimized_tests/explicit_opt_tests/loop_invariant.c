// Check copy propagation use of loop invariants are handled correctly (assuming -O1 flag).

int main() {
	int i;
	
	int cnt;
	cnt = 0;
	
	int CONST;
	CONST = 3;

	i = 0;
	while((i < 3) & (CONST == 3)) { // OPT: (i < 3) & 1
		i = i + 1;
		cnt = cnt + 1;
	}

	i = 0;
	while(i < 3) {
		int foo;
		foo = CONST + 7; // OPT: foo = 10
		
		i = i + 1;
		cnt = cnt + 1;
	}
	
	return cnt == 6;
}