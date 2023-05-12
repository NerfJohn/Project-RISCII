// Check various "unreachable statements" are handled correctly (deleted by -O1 flag).

int ONE;

int deadIf() {
	if (ONE) {
		int foo;
		foo = 1;
		
		return foo;
		
		foo = 0; // OPT: Delete.
	}
	
	return 0;
}

int deadWhile() {
	while (ONE) {
		int foo;
		foo = 1;
		
		return foo;
		
		foo = 0; // OPT: Delete.
	}
	
	return 0;
}

int main() {
	ONE = 1;
	
	int act;
	act = 1;
	
	act = act & deadIf();
	act = act & deadWhile();
	
	return ONE == act;
	
	return 0; // OPT: Delete
	
	
}