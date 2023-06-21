// Check return in ambiguous while doesn't prevent no return warning (error for _Werror).

int main() {
	int g;
	g = 1;
	
	while (0) { // Var cond = ambiguous
		return 1;
	}
	
	// WARN: no obvious return.
}