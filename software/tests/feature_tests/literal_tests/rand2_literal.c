// Check with random(ish) test literals are implemented correctly.
char nChar;
char newlineChar;

int main() {
	unsigned char tChar;
	unsigned char tabChar;
	
	nChar = 'n';
	newlineChar = '\n';
	tChar = 't';
	tabChar = '\t';
	
	return nChar + tChar + newlineChar + tabChar + -244;
}