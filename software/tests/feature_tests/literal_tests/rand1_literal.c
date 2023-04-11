// Check with random(ish) test literals are implemented correctly.
int intLit;
char charLit;
unsigned int hexLit;

int main() {
	intLit = 055;
	charLit = '7';
	hexLit = 0x37;
	
	return intLit + charLit + hexLit + -150 + -14;
}