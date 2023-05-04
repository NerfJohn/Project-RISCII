// Check warning due to constant as conditional prevents compilation (w/ -Werror).

int main() {
	// WARN: constant conditional.
	if (1) {
		int a;
		a = 3;
	}
	
	return 1;
}