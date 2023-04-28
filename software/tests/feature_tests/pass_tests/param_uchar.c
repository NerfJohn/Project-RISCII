// Check unsigned char parameters are properly passed proper values.
unsigned char min1;
unsigned char min2;
unsigned char min3;
unsigned char max1;
unsigned char max2;
unsigned char max3;
unsigned char max4;

int mins(unsigned char p1, unsigned char p2, unsigned char p3) {
	min1 = p1;
	min2 = p2;
	min3 = p3;
	
	return 0;
}

int maxes(unsigned char p1, unsigned char p2, unsigned char p3, unsigned char p4) {
	max1 = p1;
	max2 = p2;
	max3 = p3;
	max4 = p4;
	
	return 0;
}

int main() {
	mins(-32768, -128, 0);
	maxes(127, 255, 32767, 65535);
	
	// 0 + 128 + 0 + 127 + 255 + 255 + 255 = 1020
	return min1 + min2 + min3 + max1 + max2 + max3 + max4 + -1019;
}