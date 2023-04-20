// Check assignment with random(ish) test case.
unsigned char _global;

int main() {
	char errCode;
	
	errCode = -1;
	
	_global=errCode;
	
	// Check recast to unsigned char (and int for bool op) is done.
	return (_global == 255) & (_global != -1);
}