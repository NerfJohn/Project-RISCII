// Check a char literal with multiple chars is considered an error.

int main() {
	int c;
	
	// ERR: char literal cannot have two chars.
	c = 'we';
	
	return 1;
}