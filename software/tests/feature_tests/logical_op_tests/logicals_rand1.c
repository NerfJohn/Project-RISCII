// Check logical operators with "random" use case.
unsigned int byteInArray;
unsigned char charMask;

int main() {
	char shiftedChar;
	int result;
	
	byteInArray = 34;
	charMask = 0x2f;
	
	shiftedChar = byteInArray << 0x3;
	result = charMask & shiftedChar;
	
	return result;
}