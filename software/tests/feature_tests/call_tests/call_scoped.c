// Check function names also respond to scoping rules.
int name1(char param1) {return 1;}

char name2(int name1, char param1) {return 1;}

int main() {
	char name1;
	
	name1 = 0;
	name1 = name1 + name2(1,1);
	return name1;
}