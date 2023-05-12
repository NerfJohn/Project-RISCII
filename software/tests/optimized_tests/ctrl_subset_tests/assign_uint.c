// Check assignment properly sets unsigned int variables.
int main() {
	unsigned int min1;
	unsigned int min2;
	unsigned int min3;
	unsigned int max1;
	unsigned int max2;
	unsigned int max3;
	unsigned int max4;
	
	min1 = -32768; 	// Becomes 32768
	min2 = -128;	// Becomes 65408
	min3 = 0;
	max1 = 127;
	max2 = 255;
	max3 = 32767;
	max4 = 65535;
	
	// 32768 + 65408 + 0 + 127 + 255 + 32767 + 65535 = 65536 * 3 + 252
	return min1 + min2 + min3 + max1 + max2 + max3 + max4 + -251;
}