// Test parsing errors using random(ish) test case.

int main() {
	int a;
	int b;
	
	a = 3;
	b = 2;
	
	// ERR: nonsensical place for operator.
	| b;
	
	return 1;
}