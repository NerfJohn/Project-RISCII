// Check var/func warnings with random(ish) test case.

int main() {
	int cond;
	char ret;
	
	ret = 1;
	
	// WARN: Uninitialized var.
	if (cond) {
		ret = 3;
	}
	
	return 1;
}