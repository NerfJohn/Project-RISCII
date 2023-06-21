// Check if statement with random(ish) test case.
int main() {
	char fileChar;
	
	fileChar = 0xA5;
	
	if (fileChar & 0x1) {
		if (fileChar & 0x4) {
			if (fileChar & 0x20) {
				char mask;
				mask = 0x0A;
				if (fileChar & (mask << 8)) {
					if (fileChar & mask) {
						if(1) {
							fileChar = 0;
						}
					}
				}
			}
		}
	}
	
	return fileChar - 0xFFA4;
}