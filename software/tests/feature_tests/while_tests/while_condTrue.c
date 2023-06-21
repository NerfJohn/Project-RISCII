// Check while statements are skipped for true conditions (besides just 1).
// (Use functions to break out- just checking they're entered.)
int two() {
	while(2) {
		return 1;
	}
	
	return 0;
}

int negOne() {
	while(-1) {
		return 1;
	}
	
	return 0;
}

int q() {
	while('q') {
		return 1;
	}
	
	return 0;
}

int main() {
	int res;
	res = 0;
	
	res = two();
	res = res + negOne();
	res = res + q();
	
	return res - 2;
}