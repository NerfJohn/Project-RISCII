// Check variable types can be set to their max values via int literals.
unsigned int max65535;
char max127;

int main() {
	unsigned char max255;
	int max32767;
	
	max65535 = 65535;
	max127 = 127;
	max255 = 255;
	max32767 = 32767;
	
	return max65535 + max127 + max255 + max32767 + 32389;
}