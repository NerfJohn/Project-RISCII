// Check char literal with missing tick is considered an error.

int main() {
	char c;
	
	c = 'w;
	
	return 1;
}