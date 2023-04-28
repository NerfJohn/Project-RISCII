// Check unsigned char types return correctly from functions (assume params work).
unsigned char ret(int value) {return value;}

int main() {
	// 0 + 128 + 0 + 127 + 255 + 255 + 255 = 1020
	return ret(-32768) + ret(-128) + ret(0) + ret(127) + ret(255) + ret(32767) + ret(65535) + -1019;
}