// Check multiple return statements in one function are allowed/treated properly.
int foo(int arg) {
	int value;
	value = 5;
	
	if (arg > value) {
		return 1;
	}
	
	return 0;
}

int main() {
	return foo(6);
}