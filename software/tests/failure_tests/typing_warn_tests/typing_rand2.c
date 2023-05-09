// Check typing warnings using random(ish) test case.

char foo(char p1) {return p1;}

int main() {
	// WARN: 0x10000 can't fit into int or uint.
	foo(0x10000)
	
	return 1;
}