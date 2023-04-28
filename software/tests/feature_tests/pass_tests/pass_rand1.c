// Check passing behavior with random(ish) test case.
int shift8bit(unsigned int num, unsigned char shifter) {
	unsigned int result;
	
	result = num << shifter;
	
	if (result > 255) {
		return -1;
	}
	
	return result;
}

int main() {
	int res1;
	int res2;
	
	res1 = shift8bit(0x1, 0x101);	// 2
	res2 = shift8bit(0xff, 0xf);	// -1
	
	return res1 + res2;
}