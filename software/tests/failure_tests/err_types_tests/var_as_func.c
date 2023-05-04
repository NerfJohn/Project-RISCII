// Check using var identifier as function causes an error.

int main() {
	int foo;
	int bar;
	
	foo = bar();
	
	return 1;
}