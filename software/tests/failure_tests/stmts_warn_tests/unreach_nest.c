// Check unreachable lines in a nested scope cause a warning (error for -Werror).

int main() {
	int go;
	
	go = 2;
	
	if (go) {
		while (go) {
			if (go) {
				return 1;
				
				// WARN: unreachable.
				go = 0;
			}
		}
	}
	
	return 0;
}