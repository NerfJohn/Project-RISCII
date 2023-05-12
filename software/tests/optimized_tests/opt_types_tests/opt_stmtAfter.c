// Check unreachable statements are implemented correctly (assumes -O1 flag).

int main() {
	int ret;
	
	ret = 1;
	
	return ret;
	
	// OPT: Delete unreachable code.
	ret = 0;
	return ret;
}