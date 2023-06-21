// Check function calls using random(ish) test case.
int A;
int B;

int sumAB(int dummyA, int dummyB) {
	int result;
	result = A + B;
	return result;
}

int diffAB(int dummyA, int dummyB) {
	return A - B;
}

int main() {
	A = 12;
	B = 3;
	return sumAB(1,1) + diffAB(1,2) - 23;
}