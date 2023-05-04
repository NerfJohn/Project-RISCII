// Checks a bad token is considered an error.

int main() {
	int a;
	
	// ERR: bad token (char can't be 2 bytes).
	a = 'ab';
	
	return 1;
}