// Check compiler can properly access local variables, even when access is un-ideal.

// Load/store instruction have about 15 slots worth of direct access in both directions.
// Having too many local vars or parameters may require compiler to generate new pointers for accessing.
// Attempt to stress test both directions of acessing.

int bar(int p1, int p2, int p3, int p4, int p5,
		int p6, int p7, int p8, int p9, int p10,
		int p11, int p12, int p13, int p14, int p15,
		int p16) {
	int a1;
	int a2;
	int a3;
	int a4;
	int a5;
	int a6;
	int a7;
	int a8;
	int a9;
	int a10;
	int a11;
	int a12;
	int a13;
	int a14;
	int a15;
	int a16;
	
	a1 = p1;
	a2 = p2;
	a3 = p3;
	a4 = p4;
	a5 = p5;
	a6 = p6;
	a7 = p7;
	a8 = p8;
	a9 = p9;
	a10 = p10;
	a11 = p11;
	a12 = p12;
	a13 = p13;
	a14 = p14;
	a15 = p15;
	a16 = p16;
	
	return a1 + a2 + a3 + a4 + a5 +
			a6 + a7 + a8 + a9 + a10 +
			a11 + a12 + a13 + a14 + a15 + a16;
}

int main() {
	return bar(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16) - 135;
}