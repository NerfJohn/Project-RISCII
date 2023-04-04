// Check logical operators with "random" use case.
unsigned int xorMask;
unsigned int dontCares;

int main() {
	int rawData;
	int result;
	
	rawData = 0x12AB;
	dontCares = 127;
	xorMask = 'c' << 8;
	
	result = xorMask ^ rawData;
	result = ~result;
	
	return result + 29101;
}