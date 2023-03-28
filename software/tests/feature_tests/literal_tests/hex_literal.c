// Check hex literals are accepted by the language.
int fourDigits;
char oneDigit;
int blankTop;

unsigned int withChars;
unsigned char numCharMix;

int main() {
	fourDigits = 0x9090;
	oneDigit = 0x0;
	blankTop = 0x0002;
	
	withChars = 0xafAF;
	numCharMix = 0xC3;
	
	return fourDigits + oneDigit + blankTop + withChars + numCharMix + 0xbefd;
	
}