// Check constant "while" conditionals are handled correctly (inlined or pruned for -O1 flag).

// Globals- prevent TOO much optimization.
int ONE;
int ZERO;

int while0() {
	// OPT: Delete entire block.
	while(0) {return ZERO;}
	
	return ONE;
}

int while1() {
	// OPT: Inline block (+loop).
	while(1) {return ONE;}
	
	return ZERO;
}

int whileChar() {
	// OPT: Delete entire block.
	while('\0') {return ZERO;}
	
	return ONE;
}

int whileHex() {
	// OPT: Inline block (+loop).
	while(0x30) {return ONE;}
	
	return ZERO;
}


int main() {
	ONE = 1;
	ZERO = 0;
	
	int ret;
	ret = 0;
	
	ret = ret | while0();
	ret = ret | (while1() << 1);
	ret = ret | (whileChar() << 2);
	ret = ret | (whileHex() << 3);
	
	return ret == 0xA;
}