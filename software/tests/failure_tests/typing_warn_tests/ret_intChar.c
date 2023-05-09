// Check returning a int from a char function causes a warning (error for -Werror).

// WARN: Returning 'int' from 'char' func.
char foo(int ret) {return ret;}

int main() {
	int a;
	
	a = 10;
	
	foo(a);
	
	return 1;
}