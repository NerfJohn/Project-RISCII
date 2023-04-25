// Check if statements accept (and abide by) conditional expressions.
int main() {
	int a;
	int b;
	
	int res;
	
	a = 15;
	b = 23;
	
	res = 0;
	
	if (a <= b) {
		res = res | 0x1;
	}
	
	if (a + -15) {
		res = res | 0x2;
	}
	
	if (b) {
		res = res | 0x4;
	}
	
	// res = 0b101
	return res - 4;
}