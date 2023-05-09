// Check uninitialized var used as an argument causes a warning (error for -Werror).

int foo(int i) {return i;}

int main() {
	int i;
	
	// WARN: uninitialized var 'i' used.
	foo(i);
	
	return 1;
}