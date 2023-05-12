// Check explicit optimizations (assuming -O1 flag) using random(ish) test case.

// OPT: foobar() = 1
char foobar() {
	char meep;
	
	meep = 1;
	
	if (meep) { // OPT: Inline block
		return meep; // OPT: return 1
	}
	
	return 0; // OPT: Delete
}

int main() {
	int value;
	
	value = 2 + foobar(); // OPT: value = 3
	
	return value == 3; // OPT: return 1
}