// Check bidirectional boolean operators are bidirectional.
int main() {
	int eq1;
	int eq2;
	int neq1;
	int neq2;
	
	eq1 = 0xa == '\n';
	eq2 = '\n' == 0xa;
	
	neq1 = 0x0 != '0';
	neq2 = '0' != 0x0;
	
	// All 1s.
	return eq1 + eq2 + neq1 + neq2 + -3;
}