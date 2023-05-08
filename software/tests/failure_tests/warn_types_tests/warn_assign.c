// Check warning due to assignment mismatch prevents compilation (w/ -Werror).

// Note: "Downcast" warnings use slightly different type logic than translation:
// 1) Operations on same types produce same type, even if char/uchar
// 2) char/uchar operation produces int type
// 3) Literals conform to other operand's type (regardless of value)
// 4) Otherwise, operations assume uint > int > char/uchar for produced type
//
// Warning produced when lhs type != rhs type (and can't be upcasted).

// (Almost certaintly different than C/GCC, but within a "subset" context.)
// (Warnings demonstrate understanding of type checking more than harm it.)

int main() {
	int a;
	char b;
	
	a = 300;
	
	// WARN: assigning 'char' with 'int' type!
	c = a;
	
	return 1;
}