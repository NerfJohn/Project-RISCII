// Check fake escaped character is considered an error.

int main() {
	char c;
	
	// ERR: \q is not a valid escaped char.
	c = '\q';
	
	return 1;
}