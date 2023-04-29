// Check while statements are skipped for "true = 1" conditions.
// (Use functions to break out- just checking they're entered.)
int intLit() {
	while(1) {
		return 1;
	}
	
	return 0;
}

int hexLit() {
	while(0x1) {
		return 1;
	}
	
	return 0;
}

int main() {
	int res;
	res = 0;
	
	res = intLit();
	res = hexLit();
	
	return res - 1;
}