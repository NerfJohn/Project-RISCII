// Check function call, as an expression, can be a call argument.
int foo(int i) {return 1;}

int main() {
	return foo(foo(foo(3)));