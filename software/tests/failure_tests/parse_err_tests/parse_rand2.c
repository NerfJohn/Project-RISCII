// Test parsing errors using random(ish) test case.

// ERR: Missing params.
int foo {return 1;}

int main() {
	return foo();
}