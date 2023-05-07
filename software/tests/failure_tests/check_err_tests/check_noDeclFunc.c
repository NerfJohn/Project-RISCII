// Check non-declared function causes an error.

int main() {
	int a;
	
	// ERR: Undeclared function.
	a = bar();
	
	return 1;
}