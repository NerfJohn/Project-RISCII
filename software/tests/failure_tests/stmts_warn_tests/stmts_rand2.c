// Check statement warnings using a random(ish) test case.

unsigned char bar(int cnt) {
	while(cnt > 0) {
		int foo;
		
		foo = cnt - 2;
		return foo;
		
		// WARN: unreachable.
		cnt = cnt - 1;
	}
	
	return 0;
}

int main() {
	unsigned char res;
	
	res = foo(3);
	
	return res;
}