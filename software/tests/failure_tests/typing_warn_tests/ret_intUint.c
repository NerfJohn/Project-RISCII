// Check returning a int from an unsigned int function causes a warning (error for -Werror).

// WARN: Returning 'int' from 'uint' func.
unsigned int foo(int ret) {return ret;}

int main() {
	int a;
	
	a = 0x10;
	
	foo(a);
	
	return 1;
}