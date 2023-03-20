// Same/similar declarations- checks 4 types of vars can handle similar names.
// Note: Names can't be shared in scope, regardless of type- but between scopes.
int foo;
unsigned char foo1;

int main() {
	unsigned int foo;
	char foo1;
	return 1;
}