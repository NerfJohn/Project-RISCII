// Check detection/use of "loop invariants" is implemented correctly (assumes -O1 flag).

int main() {
	int i;
	int INV;
	
	i = 0;
	INV = 1;
	
	// OPT: Remove constant INV from conditional.
	while ( (i < 3) & INV) {
		i = i + 1;
	}
	
	return INV;
}