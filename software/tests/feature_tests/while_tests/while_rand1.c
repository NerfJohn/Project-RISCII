// Check while statements using random(ish) test case.
int main() {
	int i;
	int j;
	int k;
	int res;
	
	res = 0;
	i = 0;
	j = 0;
	k = 0;
	
	while (i < 2) {
		j = 0;
		while (j < 3) {
			k = 0;
			while (k < 2) {
				res = res + 1;
				k = k + 1;
			}
			j = j + 1;
		}
		i = i + 1;
	}
	
	return res - 11;
}