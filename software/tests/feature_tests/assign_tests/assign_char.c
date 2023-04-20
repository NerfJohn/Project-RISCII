// Check assignment properly sets char variables.
int main() {
	char min1;
	char min2;
	char min3;
	char max1;
	char max2;
	char max3;
	char max4;
	
	min1 = -32768; 	// Becomes 0
	min2 = -128;
	min3 = 0;
	max1 = 127;
	max2 = 255;		// Becomes -1
	max3 = 32767;	// Becomes -1	
	max4 = 65535;	// Becomes -1
	
	// 0 + -128 + 0 + 127 + -1 + -1 + -1 = -4
	return min1 + min2 + min3 + max1 + max2 + max3 + max4 + 5;
}