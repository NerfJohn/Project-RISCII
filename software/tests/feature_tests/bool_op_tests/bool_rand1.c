// Check boolean operators with random(ish) test case.
int expected;

int main() {
	int measured;
	int actual;
	
	expected = 5;
	actual = 5;
	
	char isHigh;
	char isCorrect;
	char isLow;
	
	isHigh = actual > expected;
	isCorrect = actual == expected;
	isLow = actual < expected;
	
	return isHigh + isCorrect + isLow;
}