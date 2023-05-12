// Check if statements can be nested (and accessed as expected).
int main() {
	int res;
	
	res = 0;
	
	if (1) {
		if(1) {
			res = res | 0x1;
		}
	}
	
	if (1) {
		if (0) {
			res = res | 0x2;
		}
	}
	
	if (0) {
		if(1) {
			res = res | 0x4;
		}
	}
	
	if (0) {
		if (0) {
			res = res | 0x8;
		}
	}
	
	// res = 0b0001.
	return res;
}