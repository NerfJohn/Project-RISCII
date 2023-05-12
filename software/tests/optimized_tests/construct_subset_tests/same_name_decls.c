// Checks vars with the same name/type (in different scopes) is okay.
int nameA;
char nameB;

int main() {
	int nameA;
	unsigned char nameB;
	
	return 1;
}