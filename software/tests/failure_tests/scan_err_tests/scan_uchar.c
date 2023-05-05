// Check non-std. ASCII character is considered an error (i.e. character in 128-255 range).

int main() {
	int q;
	
	// ERR: Bad character (0x90, non-std. ASCII character).
	q = 'ê';
	
	return 1;
}