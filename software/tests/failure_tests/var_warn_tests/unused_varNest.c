// Check unused var within nested scope causes a warning (error for -Werror).

int main() {
	int a;
	
	a = 1;
	
	if (a) {
		if (a) {
			while (a) {
				// WARN: unused.
				int VERY_IMPORTANT;
				
				a = 0;
			}
		}
	}
	
	return 1;
}