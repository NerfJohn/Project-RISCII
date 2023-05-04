// Check warning due to argument mismatch prevents compilation (w/ -Werror).

int foo(int ret) {return ret;}

int main() {
	unsigned int arg;
	
	arg = 6;
	
	// WARN: param 'int' given arg 'uint' type.
	foo(arg);
	
	return 1;
}