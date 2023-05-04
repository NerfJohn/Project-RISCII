// Check warning due to return mismatch prevents compilation (w/ -Werror).

// WARN: 'uchar' function returns 'char'.
unsigned char foo(char ret) {return ret;}

int main() {
	char arg;
	
	arg = 1;
	
	foo(arg);
	
	return 1;
}