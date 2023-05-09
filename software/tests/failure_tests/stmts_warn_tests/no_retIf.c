// Check return in ambiguous if doesn't prevent no return warning (error for _Werror).

int main() {
	int g;
	
	g = 1;
	
	if (g) { // Var cond = ambiguous
		return 1;
	}
	
	// WARN: no obvious return.
}