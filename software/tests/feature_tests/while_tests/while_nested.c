// Check while statements can be nested (and accessed as expected).
int main() {
	int res;
	int flag;
	
	res = 0;
	flag = 0;
	
	// 00
	while (flag) {
		while (flag) {
			res = res | 0x1;
		}
	}
	
	// 01
	while (flag) {
		while (!flag) {
			res = res | 0x2;
			flag = !flag;
		}
		flag = !flag;
	}
	
	// 10
	while (!flag) {
		while (flag) {
			res = res | 0x4;
		}
		flag = !flag;
	}
	flag = !flag;
	
	// 11
	while (!flag) {
		while (!flag) {
			res = res | 0x8;
			flag = !flag;
		}
	}
	
	// 0b1000
	return res - 7;
}