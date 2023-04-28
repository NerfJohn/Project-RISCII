// Check return statements skip over statements after the return.
int foo() {
	int value;
	
	value = 1;
	return value;
	value = 0;
}

int main() {
	return foo();
}