// Check if statement with random(ish) test case.
int main() {
	unsigned char trig;
	int err;
	
	trig = 0;
	err = 0;
	
	if (!trig) {
		char c = 'a';
		if (c == 'c') {
			trig = 1;
		}
		
		unsigned char trig;
		trig = 1;
	}
	
	if (!trig) {
		char c = 'b';
		if (c == 'c') {
			trig = 1;
		}
		
		unsigned char trig;
		trig = 1;
	}
	
	if (!trig) {
		char c = 'c';
		if (c == 'c') {
			trig = 1;
		}
		
		unsigned char trig;
		trig = 1;
	}
	
	if (!trig) {
		err = 1;
	}
	
	return (trig == 1) & (err == 0);
}