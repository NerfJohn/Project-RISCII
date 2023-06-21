// Check compiler successfully jumps over larger instruction distances.

// BRC acts as conditional jump for RISCII's ISA.
// BRC has an effective jump range of about 130 instructions.
// Unknown jumps translate using macro (which handles all distances), but nevertheless a good test.

int foo() {return 1;}

int main() {
	int res;
	
	res = 1;
	
	// Larger amount of instructions to jump over.
	if (!res) {
		int a;
		int b;
		int c;
		a = 10;
		b = -30;
		c = 20;
		
		a = a + b - c;
		b = (a | b) + 3;
		c = 1 + a + 10 + b - 40 + c;
		
		int d;
		d = a | b | c | 0x0;
		d = foo();
		a = foo();
		b = foo();
		c = foo();

		a = 10;
		b = -30;
		c = 20;
		
		a = a + b - c;
		b = (a | b) + 3;
		c = 1 + a + 10 + b - 40 + c;
		
		d = a | b | c | 0x0;
		d = foo();
		a = foo();
		b = foo();
		c = foo();
		
		a = 10;
		b = -30;
		c = 20;
		
		a = a + b - c;
		b = (a | b) + 3;
		c = 1 + a + 10 + b - 40 + c;
		
		d = a | b | c | 0x0;
		d = foo();
		a = foo();
		b = foo();
		c = foo();

		a = 10;
		b = -30;
		c = 20;
		
		a = a + b - c;
		b = (a | b) + 3;
		c = 1 + a + 10 + b - 40 + c;

		d = a | b | c | 0x0;
		d = foo();
		a = foo();
		b = foo();
		c = foo();
		
		res = 0; // Reset return value to establish failure
	}
	
	return res;
}