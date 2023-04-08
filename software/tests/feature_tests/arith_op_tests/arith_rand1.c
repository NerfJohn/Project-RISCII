// Check arithmetic operators with random(ish) test case.
int sum;
int diff;

int main() {
	unsigned int param1;
	char param2;
	
	sum = 0;
	diff = 0;
	
	param1 = 47;
	param2 = -8;
	
	sum = sum + 10 + 19 + -40 + 39 + -1023 + 1045 + param1 + param2;
	diff = diff - 10 - 19 - -40 - 39 - -1023 - 1045 - param1 - param2;
	
	// 89 + -89 = 0
	return sum + diff + 1;
}