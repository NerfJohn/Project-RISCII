// Check operator ordering with random(ish) case.
int main() {
	int calcVal;
	
	calcVal = 0xf & 0xfd + ~0xFFFA;
	return calcVal > 1 == calcVal < 3;
}