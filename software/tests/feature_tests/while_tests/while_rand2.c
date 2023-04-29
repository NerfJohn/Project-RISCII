// Check while statements using random(ish) test case.
int main() {
	unsigned int bitStream;
	int bitCnt;
	
	bitStream = 0xA50E;
	bitCnt = 0;
	
	while (bitStream != 0x0) {
		int i;
		i = 0;
		while(i < 4) {
			bitCnt = bitCnt + (bitStream & 0x1);
			bitStream = bitStream >> 1;
			i = i + 1;
		}
	}
	
	return bitCnt == 7;
}