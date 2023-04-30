// Example program of a 1-D convolution across 16 bits.

// Peform 1-D convolution: "AoE" of 6, stride of 2, operation is XOR.
unsigned char conv(unsigned int image) {
	unsigned char result;
	result = 0;
	
	// "AoE" of 6, stride of 2 = 6 bits to generate.
	int i;
	i = 0;
	while(i < 6) {
		// Shift down bits in question.
		unsigned char bits;
		bits = (image >> (10 - (i << 1)));
		
		// Determine XOR bit for AoE.
		result = result << 1; // Prep for next bit.
		int j;
		j = 0;
		while (j < 6) {
			result = result ^ ((bits >> j) & 0x1);
			j = j + 1;
		}
		
		i = i + 1; // increment
	}
	
	return result;
}

int main() {
	int actual;
	int expected;
	
	actual = conv(0xbeef);
	expected = 0x3B;
	
	return actual == expected;
}