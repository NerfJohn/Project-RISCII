// Check function calls using random(ish) test case.
int numCalls;
int callsLeft;

int helper() {
	numCalls = numCalls + 1;
	callsLeft = callsLeft - 1;
	if (callsLeft) {helper();}
	return 0;
}

int kickoff(int i) {
	numCalls = numCalls + 1;
	callsLeft = 3;
	helper();
	
	return 1;
}

int main() {
	kickoff(kickoff(1));
	
	return numCalls - 7;
}