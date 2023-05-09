// Check char literal as a conditional causes a warning (error for -Werror).

int main() {
	// WARN: constant conditional of 'S'.
	if ('S') {
		return 1;
	}
	
	return 0;
}