// Check arithmetic operators can use a mix of variable and literal operands.
int main() {
	int i;
	char c;
	unsigned int ui;
	unsigned char uc;
	
	i = 25;
	c = -6;
	ui = 4096;
	uc = 127;
	
	int add1;
	int sub1;
	int minus1;
	int add2;
	int sub2;
	int minus2;
	
	add1 = 0x45 + c;
	add2 = ui + '\t';
	sub1 = 100 - uc;
	sub2 = 0xffff - ui;
	minus1 = -'\0';
	minus2 = -i;
	
	// 63 + 4105 + -27 + 61439 + 0 + -25 = 65536 + 19
	return add1 + add2 + sub1 + sub2 + minus1 + minus2 + -18;
}