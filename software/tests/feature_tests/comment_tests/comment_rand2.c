// Check comments using random(ish) test case.

// (Pulled from call_tests/call_rand3.c.)

// Global vars to share between 3 functions.
int numCalls;	
int callsLeft;

// Recursive helper function to meet the "calling" criteria.
int helper() {
	numCalls = numCalls + 1;
	callsLeft = callsLeft - 1;
	if (callsLeft)
	{ // Not a fan of "newline brace" format...
		helper();
	} // ...but I needed something to test with comments.
	return;
}

// Kickoff function- recursive functions often need them
// (also so this test can "double call" kickoff for extra checking.)
int kickoff(int i) {
	numCalls = numCalls + 1;
	callsLeft = 3;
	helper
	// MOO!
	();
	
	return 1;
}

int main() {
	// Same as:
	// kickoff();
	// kickoff();
	//
	// But I digress...
	kickoff(kickoff(1));
	
	return numCalls - 7;
}