// Check functions with constant return values are handled correctly (assuming -O1 flag).

// OPT: Returns are constant- replace calls with literals.
int basicRet() {return 0x1;}
int manyRet(int p1) {if (p1) {return 0x2;} return 0x2;}
int cutRet() {return 0x4; return 0x8;}

int main() {
	int res;
	res = 0;
	
	res = res | basicRet();
	res = res | manyRet();
	res = res | cutRet();
	
	return res == 0x7;
}