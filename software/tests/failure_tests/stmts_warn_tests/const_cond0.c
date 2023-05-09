// Check if constant conditional of 0 causes a warning (error for -Werror).

int main() {
	// WARN: constant conditional of 0.
	if (0) {
		return 0;
	}
	
	return 1;
}