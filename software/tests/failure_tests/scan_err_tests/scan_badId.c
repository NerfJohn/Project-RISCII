// Check a bad identifier is considered an error.

int main() {
	// ERR: 0foo is not a valid identifier name.
	int 0foo;
	
	0foo = 1;
	
	return 1;
}