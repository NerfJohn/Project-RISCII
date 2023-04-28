// Check char parameters are properly passed proper values.
char min1;
char min2;
char min3;
char max1;
char max2;
char max3;
char max4;

int mins(char p1, char p2, char p3) {
	min1 = p1;
	min2 = p2;
	min3 = p3;
	
	return 0;
}

int maxes(char p1, char p2, char p3, char p4) {
	max1 = p1;
	max2 = p2;
	max3 = p3;
	max4 = p4;
	
	return 0;
}

int main() {
	mins(-32768, -128, 0);
	maxes(127, 255, 32767, 65535);
	
	// 0 + -128 + 0 + 127 + -1 + -1 + -1 = -4
	return min1 + min2 + min3 + max1 + max2 + max3 + max4 + 5;
}