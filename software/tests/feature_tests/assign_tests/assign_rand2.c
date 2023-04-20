// Check assignment with random(ish) test case.

unsigned int ui;
int i;
char c;

int main() {
	i = -32768;
	ui = i;
	c = i;
	
	// Check truncating/reinterpreting rules.
	return (c == 0) & (ui == 32768) & (ui == i);
}