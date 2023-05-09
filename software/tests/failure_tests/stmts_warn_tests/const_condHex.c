// Check hex literal as a conditional causes a warning (error for -Werror).

int main() {
	// WARN: constant conditional of 0xDead.
	if (0xDead) {
		return 1;
	}
	
	return 0;
}