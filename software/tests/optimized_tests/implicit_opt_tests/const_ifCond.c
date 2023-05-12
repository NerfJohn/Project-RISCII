// Check constant "if" conditionals are handled correctly (inlined or pruned for -O1 flag).

int main() {
	int ret;
	ret = 0;
	
	// OPT: Delete entire block.
	if (0) {
		ret = ret | 0x1;
	}
	
	// OPT: Inline entire block.
	if (1) {
		ret = ret | 0x2;
	}
	
	// OPT: Delete entire block.
	if ('\0') {
		ret = ret | 0x4;
	}
	
	// OPT: Inline entire block.
	if (0x10) {
		ret = ret | 0x8;
	}
	
	return ret == 0xA;
}