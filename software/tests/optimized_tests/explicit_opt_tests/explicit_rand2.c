// Check explicit optimizations (assuming -O1 flag) using random(ish) test case.

int main() {
	int limit;
	int i;
	
	limit = 5;
	
	i = 0;
	while( i < limit) { // OPT:  < 5
		int a;
		int b;
		
		a = limit << 3 	// OPT: a = 40
		b = a + 8; 		// OPT: b = 48
		
		i = i + 1;
	}
	
	return (limit + i) == 0xA; // OPT: return 1
}