// Check "copy propagation" of literals is implemented correctly (assumes -O1 flag).

int main() {
	int a;
	int b;
	
	a = 1;
	
	// (Junk to remove 'a' from register file).
	int c;
	int d;
	int e;
	int f;
	int g;
	b = c + d + e + f + g;
	
	// OPT: Replace 'a' with '1'.
	b = a;
	
	return b;
}