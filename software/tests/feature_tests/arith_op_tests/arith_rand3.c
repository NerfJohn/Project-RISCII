// Check arithmetic operators with random(ish) test case.
unsigned int sum;
int mSum;

int main() {
	char param;
	
	param = 120;
	
	sum = 4096 + -param + -0x23 + 245 + '\n';
	mSum = -sum;
	
	return sum - mSum - sum + mSum + 1;
}