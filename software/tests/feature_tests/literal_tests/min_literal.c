// Check variable types can be set to their min values via int literals.
int min_neg_32768;
unsigned int min_0_int;

int main() {
	unsigned char min_0_char;
	char min_neg_128;
	
	min_neg_32768 = -32768;
	min_0_int = 0;
	min_0_char = 0;
	min_neg_128 = -128;
	
	return min_neg_32768 + min_0_int + min_0_char + min_neg_128 + -32639;
}