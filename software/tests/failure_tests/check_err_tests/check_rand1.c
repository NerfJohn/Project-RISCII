// Check errors during "check phases" using random(ish) test case.

int foo(int paramName) {return paramName;}

int main() {
	int a;
	
	a = 3;
	
	// ERR: paramName not declared in this scope.
	a = paramName;
	
	return 1;
}