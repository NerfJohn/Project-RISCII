// Check logical operators with "random" use case.
unsigned int byteInArray;
unsigned char charMask;

int main() {
	char shiftedChar;
	int result;
	
	byteInArray = 34;
	charMask = 0xef;
	
	shiftedChar = byteInArray << 0x3;
	result = charMask & shiftedChar;
	
	return result + 1;
}