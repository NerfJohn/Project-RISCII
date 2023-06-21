// Check parameters obey scoping rules (and act as writeable var in a function).

int name;

int foo(int name) {
	name = 0;
	return name;
}

int main() {
	int res;
	name = 1;
	res = foo(2);
	return name + res;
}