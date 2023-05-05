// Check Std. ASCII character not part of the language is considered an error.

int main() {
	int q;
	
	// ERR: Bad character (0x8, backspace character).
	q = '';
	
	return 1;
}