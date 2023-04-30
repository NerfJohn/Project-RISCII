// Checks compiler can handle "levels" of operator usage.

// Operator instructions have both register and immediate forms.
// Compiler could use either (and should know when it can't use one of the other).
// Forms can be invoked by size of immediate operand.

int main() {
	int operand;
	operand = 10;
	
	int and1;
	int and2;
	and1 = operand & 15;
	and2 = operand & 100;
	
	int or1;
	int or2;
	or1 = operand | 15;
	or2 = operand | 100;
	
	int xor1;
	int xor2;
	xor1 = operand ^ 15;
	xor2 = operand ^ 100;
	
	int shl1;
	int shl2;
	shl1 = operand << 10;
	shl2 = operand << operand; // Enough bits in instruction to always embed immediate
	
	int shr1;
	int shr2;
	shr1 = operand >> 10;
	shr2 = operand >> operand; // Enough bits in instruction to always embed immediate
	
	int shra1;
	int shra2;
	shra1 = -300 >> 10;
	shra2 = -300 >> operand; // Enough bits in instruction to always embed immediate
	
	int add1;
	int add2;
	add1 = operand + 15;
	add2 = operand + 100;
	
	int sub1;
	int sub2;
	sub1 = operand + 15;
	sub2 = operand + 100;
	
	int lit1;
	int lit2;
	int lit3;
	lit1 = 127;
	lit2 = -128;
	lit3 = 300;
	
	// and1 + ... = 21297.
	return and1 + and2 + or1 + or2 + xor1 + xor2 + 
			shl1 + shl2 + shr1 + shr2 + shra1 + shra2 + 
			add1 + add2 + sub1 + sub2 + lit1 + lit2 + lit3 - 21296;
}