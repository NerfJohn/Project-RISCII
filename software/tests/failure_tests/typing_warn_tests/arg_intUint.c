// Check passing a int to a unsigned int causes a warning (error for -Werror).

unsigned int foo(unsigned int a) {return a;}

int main() {
	int a;
	
	a = 300;
	
	// WARN: param 'uint' given 'int'.
	foo(a);
	
	return 1;
}