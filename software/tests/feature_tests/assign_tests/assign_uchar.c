// Check assignment properly sets unsigned char variables.
int main() {
	unsigned char min1;
	unsigned char min2;
	unsigned char min3;
	unsigned char max1;
	unsigned char max2;
	unsigned char max3;
	unsigned char max4;
	
	min1 = -32768; 	// Becomes 0
	min2 = -128;	// Becomes 128
	min3 = 0;
	max1 = 127;
	max2 = 255;
	max3 = 32767;	// Becomes 255
	max4 = 65535;	// Becomes 255
	
	// 0 + 128 + 0 + 127 + 255 + 255 + 255 = 1020
	return min1 + min2 + min3 + max1 + max2 + max3 + max4 + -1019;
}