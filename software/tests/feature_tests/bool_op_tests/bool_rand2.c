// Check boolean operators with random(ish) test case.

int neg;
int pos;

int main() {
	neg = -10;
	pos = 5;
	
	unsigned char geq;
	
	geq = neg >= pos;
	
	return !geq;
}