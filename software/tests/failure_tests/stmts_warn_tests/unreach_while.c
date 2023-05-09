// Check unreachable lines in a while block cause a warning (error for -Werror).

int main() {
	int cond;
	
	cond = 1;
	
	while (cond) {
		cond = -1;
		return 1;
		
		// WARN: unreachable.
		cond = 0;
	}
	
	return 1;
}