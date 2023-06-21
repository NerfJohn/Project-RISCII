// Check passing behavior with random(ish) test case.
unsigned int toUint(int ui) {return ui;}

unsigned char toUchar(char uc) {return uc;}

char toChar(int i) {return i;}

int main() {
	int a;
	a = toUint(32768) + toUint(-32768);
	
	int b;
	b = toUchar(0x8000) + toChar(256);
	
	return a + b + 1;
}