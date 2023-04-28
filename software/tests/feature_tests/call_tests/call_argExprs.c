// Check function call (with expression as args) can act as an expression.
int foo(int i) {return 1;}

int bar(unsigned char uc, unsigned int ui, char c) {return 1;}

int main() {
	int foobar;
	foobar = 12;
	return foo(0x3 & 3) + bar(45 + 't',foobar,foobar + 3 & 0xff) - 1;
}