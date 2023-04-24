// Check if statements are skipped for true conditions (besides just 1).
int main() {
	int res;
	
	res = 0;
	
	if (2) {
		res = res + 1;
	}
	
	if (-1) {
		res = res + 1;
	}
	
	if ('q') {
		res = res + 1;
	}
	
	return res - 2;
}