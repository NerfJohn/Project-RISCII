// Check passing an int to a char causes a warning (error for -Werror).

char foo(char a) {return a;}

int main() {
	int a;
	
	a = 300;
	
	// WARN: param 'char' given 'int'.
	foo(a);
	
	return 1;
}