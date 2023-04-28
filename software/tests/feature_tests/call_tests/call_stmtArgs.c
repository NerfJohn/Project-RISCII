// Check function call (with args) can act as a statement (syntax test).
int foo(int i) {return 1;}

int bar(unsigned char uc, unsigned int ui, char c) {return 1;}

int main() {
	foo(1);
	bar(1,1,1);
	return 1;
}