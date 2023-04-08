// Check arithmetic operands handle over/underflow operations correctly.
int main() {
	int add1;
	char add2;
	int sub1;
	unsigned char sub2;
	int minus1;
	unsigned int minus2;
	
	add1 = 65535 + 2; // overflow
	add2 = -128 + -4; // underflow
	
	sub1 = 65000 - -1000; // overflow
	sub2 = 10 - 20; // underflow
	
	minus1 = -0x8000; // overflow
	minus2 = -'\n'; // undeflow
	
	// 1 + 124 + 464 + 246 + -32768 + 65526 = 33593
	return add1 + add2 + sub1 + sub2 + minus1 + minus2 + -33592;
}