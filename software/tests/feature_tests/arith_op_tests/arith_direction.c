// Check arithmetic operators for bidirectional properties.
int main() {
	int add1;
	int add2;
	int sub1;
	int sub2;
	
	add1 = 10 + 5;
	add2 = 5 + 10;
	
	sub1 = 13 - 6;
	sub2 = 6 - 13;
	
	// 15 + 15 + 7 + -7 = 30
	return add1 + add2 + sub1 + sub2 + -29;
}