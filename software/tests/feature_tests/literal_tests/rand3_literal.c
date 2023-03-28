// Check with random(ish) test literals are implemented correctly.
int main() {
	int A;
	int Ab;
	int Abb;
	int Abba;
	int bba;
	int ba;
	int a;
	
	A = 0x000A;
	Ab = 0x00Ab;
	Abb = 0x0Abb;
	Abba = 0xAbba;
	bba = 0xbba0;
	ba = 0xba00;
	a = 0xa000;
	
	return A + Ab + Abb + Abba + bba + ba + a + 13111;
}