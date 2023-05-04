// Check calling a function with the wrong number of arguments causes an error.

int foo(int p1, int p2) {return p1 + p2;}

int main() {
	int ret;
	
	ret = foo(3);
	
	return ret;
}