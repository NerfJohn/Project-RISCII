// Check operator ordering with random(ish) case.
unsigned char cfgReg;
unsigned char maskLo;
unsigned char maskHi;

int main() {
	int high;
	int low;
	
	cfgReg = 0x87;
	maskLo = 0x0f;
	
	maskLo = (maskLo & cfgReg) + 12;
	maskHi = (cfgReg >> 4) + 12;
	
	return -(maskLo - maskHi);
}