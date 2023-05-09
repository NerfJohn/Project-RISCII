// Check int literal as a conditional causes a warning (error for -Werror).

int main() {
	// WARN: constant conditional of 22.
	if (22) {
		return 1;
	}
	
	return 0;
}