// Check passing behavior with random(ish) test case.
char foo(unsigned char idx) {
	if (idx == 0) {return -128;}
	if (idx == 1) {return 127;}
	if (idx == 2) {return 128;}
	return -32768;
}

int main() {
	return foo(0) + foo(1) + foo(2) + foo(3) - 126;
}