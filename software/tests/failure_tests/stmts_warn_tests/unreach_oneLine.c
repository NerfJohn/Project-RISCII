// Check one unreachable line causes a warning (error for -Werror).

int main {
	int doo;
	
	return 1;
	
	// WARN: unreachable.
	doo = 1;
}