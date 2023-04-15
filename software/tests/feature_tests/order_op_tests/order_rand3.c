// Check operator ordering with random(ish) case.
unsigned int myReg;

int main() {
	myReg = 0xAb21;
	
	int res;
	
	res = ((myReg & 0xf) == (myReg >> 8 & 0xf)) ^ 0x1;
	
	return !res;
}