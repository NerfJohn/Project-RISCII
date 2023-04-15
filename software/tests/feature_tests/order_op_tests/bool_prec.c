// Check boolean operators behave with expected precedence.
int main() {
	int result;
	
	// Precedence: <Highest> ! [> >= < <=] [== !=] <Lowest>
	
	// 1 != (1 >= 0 > (!result) < 1 <= 1) == 0;
	result = 1;
	result 1 != 1 >= 0 > !result < 1 <= 1 == 0;
	
	return result;
}