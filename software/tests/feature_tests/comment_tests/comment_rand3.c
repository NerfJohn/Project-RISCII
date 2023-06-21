// Check comments with random(ish) case.

// (Pulled from order_tests/order_rand3.c.)

////////////////////////// Glorified Global Var ////////////////////////////////
unsigned int myReg;
///////////////////////// ~Glorified Global Var ////////////////////////////////

// Main program: Does- something...
int main() {
	myReg = 0xAb21;
	
	int res;
	
	// Some check on order of operations- its been a while since writing this test.
	res = ((myReg & 0xf)) 	// Build me up
	== 						// Buttercup
	((myReg >> 8 & 0xf) 	// (Baby)
	^ 0x1);
					//   ||
	return ! // i.e. NOT \/
						res;
}