// Check arithmetic operators behave with expected precedence.
int main() {
	int result;
	
	// Precedence: <Highest> -(unary) [+ -] <Lowest>
	
	// 12 - (-result) + 3 - 30;
	result = 5;
	result 12 - -result + 3 - 30;
	
	return result + -9;
}