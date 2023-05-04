// Checks unexpected token is an error.

int main() {
	int a;
	
	// ERR: Curly bracing unexpected mid-assignment.
	a = };

	return 1;
}