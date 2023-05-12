// Check implicit optimizations (assuming -O1 flag) using random(ish) test case.

// OPT: Should delete 'retUint', 'retChar', and 'RESULT_UINT'.
int retInt(int i) {return i;}
char retChar(char c) {return c;}
unsigned int retUint(unsigned int ui) {return ui;}
unsigned char retUchar(unsigned char uc) {return uc;}
int RESULT_INT;
unsigned int RESULT_UINT;

int main() {
	
	RESULT_INT = retInt(3) & retUchar(1);
	
	return RESULT_INT;
}