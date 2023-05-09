// Check many unreachable lines causes a warning (error for -Werror).

int main {
	int doo;
	
	return 1;
	
	// WARN: all unreachable.
	int ood;
	doo = 1;
	ood = !doo;
}