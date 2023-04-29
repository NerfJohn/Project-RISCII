// Check while statements accept (and abide by) conditional expressions.
// (Use functions to break out- just checking they're entered.)
int a;
int b;

int a() {
	while(a + -15) {
		return 1;
	}
	
	return 0;
}

int b() {
	while(b) {
		return 1;
	}
	
	return 0;
}

int ab() {
	while(a <= b) {
		return 1;
	}
	
	return 0;
}

int main() {
	int res;
	res = 0;
	
	a = 15;
	b = 23;
	
	res = res | a();
	res = res | (b() << 1);
	res = res | (ab() << 2);
	
	// 0 + (1 << 1) + (1 << 2) = 6.
	return res - 5;
}