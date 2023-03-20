// Spaced declarations- checks 4 types of var declaration are space-agnostic.
int        foo;
char			bar	;
	unsigned     int  foobar;

int main() {
unsigned	char barfoo		;
	return 1;
}