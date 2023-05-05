// Check scan phase errors using random(ish) test case.

int main () {
	int foo;
	
	// ERR: ':' isn't used in this language.
	foo = 2:
	
	return 1;
}