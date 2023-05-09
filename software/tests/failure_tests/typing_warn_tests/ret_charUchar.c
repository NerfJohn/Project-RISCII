// Check returning a char from a unsigned char function causes a warning (error for -Werror).

// WARN: Returning 'char' from 'uchar' func.
unsigned char foo(char ret) {return ret;}

int main() {
	char a;
	
	a = '4';
	
	foo(a);
	
	return 1;
}