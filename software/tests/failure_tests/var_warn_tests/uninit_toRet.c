// Check returning an uninitialized var causes a warning (error for -Werror).

int foo() {
	int i;
	
	// WARN: returning uninitialized value.
	return i;
}

int main() {
	foo();
	
	return 1;
}