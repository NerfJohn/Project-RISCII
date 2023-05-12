// Check arithmetic operators can use variable operands.
int main() {
	int i;
	char c;
	unsigned int ui;
	unsigned char uc;
	
	i = 24;
	c = -3;
	ui = 1000;
	uc = 120;
	
	int add;
	int sub;
	int minus;
	
	add = i + ui;
	sub = c - uc;
	minus = -i;
	
	// 1024 + -123 + -24 = 877
	return add + sub + minus - 876;
}