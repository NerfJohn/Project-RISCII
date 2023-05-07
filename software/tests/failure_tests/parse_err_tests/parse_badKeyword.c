// Check misspelled keyword causes an error.

int main() {
	// ERR: 'char' not 'chr'.
	chr foo;
	
	foo = 1;
	
	return foo;
}