// Check while statements using random(ish) test case.
int main() {
	int res;
	res = 0;
	
	int i;
	i = 0;
	while (i < 6) {
		i = i + 1;
		
		int i;
		i = 0;
		while (i < 2) {
			i = i + 1;
		}
		res = res + 1;
	}
	
	return res - 5;
}