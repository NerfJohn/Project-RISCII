// Check hex literals are accepted by language- even if they overflow/get reinterpreted.
int main() {
	int highBitNegative;
	unsigned int highBitPositive;

	char cutOffTop;
	unsigned char alsoCutTop;
	
	// Same value, but different interpretations.
	highBitNegative = 0x8000;
	highBitPositive = 0x8000;
	
	// Truncates value- getting just lower byte.
	cutOffTop = 0xFF00;
	alsoCutTop = 0xFFFF;
	
	return highBitNegative + highBitPositive + cutOffTop + alsoCutTop + 0xff02;