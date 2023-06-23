// Check warning due to statement after return statment prevents compilation (w/ -Werror).

int main() {
	int q;
	
	return 1;
	
	// WARN: Unreachable statement.
	q = 1;
}