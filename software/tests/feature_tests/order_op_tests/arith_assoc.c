// Check arithmetic operators behave with expected associativity.
int main() {
	int add;
	int sub;
	int minus;
	
	add = 5 + 4 + 3;
	sub = 5 - 4 - 3;
	minus = - - - 9;
	
	// 12 + -2 + -9 = 1
	return add + sub + minus;
}