// Check char literals are accepted by language.
char lowChar;
char highChar;

int main() {
	unsigned int uint;
	unsigned char uchar;
	int sint;
	
	lowChar = ' ';
	highChar = '~';
	
	uint = 'a';
	uchar = 'A';
	sint = 'Z';
	
	return lowChar + highChar + uint + uchar + sint + -509 + 'd'; // -409 = -509 + (d=100)
}