// Check logical operators with "random" use case.
int main() {
	char shiftNum;
	unsigned char orMask;
	int result;
	
	orMask = 0x8000;
	shiftNum = 3;
	
	result = 0x0004 << shiftNum;
	result = result >> shiftNum;
	result = result | orMask;
	result = result >> shiftNum;
	
	return result + 1;
}