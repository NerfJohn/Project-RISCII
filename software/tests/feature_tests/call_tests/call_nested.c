// Check function calls can be "nested".
int foobar() {return 1;}
int bar() {return foobar();}
int foo() {return bar();}

int main() {
	return foo();
}