// Check if statement with random(ish) test case.
int errCode;
unsigned int result;

int main() {
	unsigned int expected;
	
	errCode = 0;
	
	expected = 5;
	result = 4;
	
	if (result > expected) {
		errCode = 1;
	}
	
	if (result < expected) {
		errCode = 2;
		expected = result;
	}
	
	if (result != expected) {
		errCode = -1;
		expected = result + 1;
	}
	
	return errCode - 1;
}