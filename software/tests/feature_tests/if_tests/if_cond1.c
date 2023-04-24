// Check if statements are skipped for "true = 1" conditions.
int main() {
	int res;
	
	res = 0;
	
	if (1) {
		res = res + 1;
	}
	
	if (0x1) {
		res = res + 1;
	}
	
	return res - 1;
}