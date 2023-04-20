// Check assignment properly sets int variables.
int main() {
	int min1;
	int min2;
	int min3;
	int max1;
	int max2;
	int max3;
	int max4;
	
	min1 = -32768;
	min2 = -128;
	min3 = 0;
	max1 = 127;
	max2 = 255;
	max3 = 32767;
	max4 = 65535;	// Becomes -1
	
	// -32768 + -128 + 0 + 127 + 255 + 32767 + -1 = 252
	return min1 + min2 + min3 + max1 + max2 + max3 + max4 + -251;
}