// Check using function identifier as variable causes error.

int foobar() {return 0;}

int main() {
	int f;
	
	f = foobar;
	
	return 1;
}