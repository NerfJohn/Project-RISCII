// Check copy propagation for statements are handled correctly (assuming -O1 flag).

int main() {
	int res1;
	int res2;
	int res3;
	
	int known;
	
	known = 2;
	
	res1 = known; 		// OPT: res1 = 2;
	res2 = known + 3; 	// OPT: res2 = 5;
	res3 = res1 + res2;	// OPT: res3 = 7;
	
	return res3 == 7;
}