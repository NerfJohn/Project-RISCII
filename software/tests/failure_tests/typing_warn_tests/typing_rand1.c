// Check typing warnings using random(ish) test case.

int main() {
	char a;
	unsigned char b;
	unsigned char c;
	
	a = 3;
	b = 5;
	
	// WARN: 'uchar' assigned with resulting 'int'.
	c = a + b;
	
	return 1;
}