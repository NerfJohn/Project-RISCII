// Check variable types can handle "underflow" by literal values.
int vInt;
char vChar;

int main() {
	unsigned int vUint;
	unsigned char vUchar;
	
	vInt = -32769; // Becomes 32767
	vChar = -129; // Becomes 127
	
	vUint = -1; // Becomes 65535
	vUchar = -1; // Becomes 255
	
	return vInt + vChar + vUint + vUchar + -33147; // Added lit underflows "int" type
}