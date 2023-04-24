// Check if statements are skipped for "false = 0" conditions.
int main() {
	int res;
	
	res = 1;
	
	if (0) {
		res = 0;
	}
	
	if ('\0') {
		res = 0;
	}
	
	if (0x0) {
		res = 0;
	}
	
	return res;
}