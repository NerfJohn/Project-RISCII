// Check statement warnings using a random(ish) test case.

int main() {
	// WARN: Const. conditional.
	if ('\0') {return 0;}
	return 1;
}