// Check literal for a while conditional causes a warning (error for -Werror).

int main() {
	// WARN: constant conditional of 0.
	while (0) {
		return 0;
	}
	
	return 1;
}