// Check while statements are skipped for "false = 0" conditions.
int main() {
	int res;
	
	res = 1;
	
	while (0) {
		res = 0;
	}
	
	while ('\0') {
		res = 0;
	}
	
	while (0x0) {
		res = 0;
	}
	
	return res;
}