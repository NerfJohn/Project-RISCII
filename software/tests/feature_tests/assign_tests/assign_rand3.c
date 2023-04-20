// Check assignment with random(ish) test case.
unsigned char MAX;

int main() {
	unsigned char c;
	int i;
	
	MAX = 255;
	
	c = MAX + 1;
	i = MAX + 1;
	
	return c + i + -255;
}