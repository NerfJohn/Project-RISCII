// Check logical operators behave with expected associativity.
int main() {
	int and;
	int or;
	int xor;
	int not;
	int shl;
	int shr;
	
	and = 0x00ff & 0x00f0 & 0x00ef;
	or = 0x1 | 0xf | 0xf0;
	xor = 0xf ^ 0x3 ^ 0x8;
	not = ~~~~0x1;
	shl = 0x1 << 1 << 2 << 3;
	shr = 0x4000 >> 1 >> 3 >> 2;
	
	// 224 + 255 + 4 + 1 + 64 + 256 = 804
	return and + or + xor + not + shl + shr + -803;
}