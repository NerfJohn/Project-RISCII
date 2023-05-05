// Check std. (visible) ASCII character that isn't used in language is considered an error.

int main() {
	int a;
	
	// ERR: '@' is unused in this language.
	a = 4@;
	
	return 1;
}