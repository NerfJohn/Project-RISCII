// Check various "unused variables" are handled correctly (deleted by -O1 flag).

unsigned char unusedGlobal; // OPT: Delete

int ONE;

int main() {
	char unusedLocal; // OPT: Delete
	
	ONE = 1;
	
	if (ONE) {
		if (ONE) {
			if (ONE) {
				int unusedNested; // OPT: Delete
				
				ONE = 1;
			}
		}
	}
	
	return ONE;
}