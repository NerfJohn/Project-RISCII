// Check while statements using random(ish) test case.
int main() {
	int i;
	int j;
	int k;
	
	res = 0;
	
	while (i < 2) {
		while (j < 3) {
			while (k < 2) {
				res = res + 1;
			}
		}
	}
	
	return res - 11;
}