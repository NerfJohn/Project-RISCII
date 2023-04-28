// Check function calls using random(ish) test case.
int globalA;
int globalB;
int PARAM;

char resetParam() {PARAM = 0; return 0;}

int initGlobals() {
	
	if (PARAM == 1) {
		globalA = 3;
		globalB = 5;
	}
	
	if (PARAM != 1) {
		globalA = -1;
		globalB = 2;
	}
	
	resetParam();
	
	return 0;
}

int main() {
	PARAM = -1;
	initGlobals();
	return globalA + globalB;
}