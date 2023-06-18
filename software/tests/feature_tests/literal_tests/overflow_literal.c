// Check variable types can handle "overflow" by literal values.
int vInt;
char vChar;

int main() {
	unsigned int vUint;
	unsigned char vUchar;
	
	vInt = 32768; // Becomes -32768
	vChar = 128; // Becomes -128
	
	vUint = 65536; // Becomes 0
	vUchar = 256; // Becomes 0
	
	return vInt + vChar + vUint + vUchar + 32897; // Added lit overflows "int" type
}