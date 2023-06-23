// Check functions can be called recursively.
int numCalls;

int recurse() {
	numCalls = numCalls + 1;
	if (numCalls < 3) {recurse();}
	return 1;
}

int main() {
	numCalls = 0;
	return recurse();
}