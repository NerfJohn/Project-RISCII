// Check unsigned int parameters are properly passed proper values.
unsigned int min1;
unsigned int min2;
unsigned int min3;
unsigned int max1;
unsigned int max2;
unsigned int max3;
unsigned int max4;

int mins(unsigned int p1, unsigned int p2, unsigned int p3) {
	min1 = p1;
	min2 = p2;
	min3 = p3;
	
	return 0;
}

int maxes(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4) {
	max1 = p1;
	max2 = p2;
	max3 = p3;
	max4 = p4;
	
	return 0;
}

int main() {
	mins(-32768, -128, 0);
	maxes(127, 255, 32767, 65535);
	
	// 32768 + 65408 + 0 + 127 + 255 + 32767 + 65535 = 65536 * 3 + 252
	return min1 + min2 + min3 + max1 + max2 + max3 + max4 + -251;
}