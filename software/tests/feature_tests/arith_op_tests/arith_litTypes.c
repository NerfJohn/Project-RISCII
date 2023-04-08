// Check arithmetic operators work with literal types.
unsigned int add1;
char sub1;
int minus1;

int main() {
	unsigned char add2;
	int sub2;
	char minus2;
	
	add1 = 0x1A + 4;
	add2 = '\0' + 0x0003;
	
	sub1 = -5 + 'g';
	sub2 = -100 + 0xe;
	
	minus1 = -0x12;
	minus2 = -'Z';
	
	// 30 + 3 + 98 + -86 + -12 + -90 = -57;
	return add1 + add2 + sub1 + sub2 + minus1 + minus2 + 58;
}