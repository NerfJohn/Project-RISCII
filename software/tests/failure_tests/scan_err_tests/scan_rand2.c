// Check scan phase errors using random(ish) test case.

int main () {
	char bar;
	
	// ERR: should be '\t', not '/t'.
	bar = '/t';
	
	return 1;
}