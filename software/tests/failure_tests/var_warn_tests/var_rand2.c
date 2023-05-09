// Check var/func warnings with random(ish) test case.

int main() {
	int val;
	
	// WARN: Uninitialized var.
	val = val << 1;
	
	return 1;
}