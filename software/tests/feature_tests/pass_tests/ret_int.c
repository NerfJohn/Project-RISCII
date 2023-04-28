// Check int types return correctly from functions (assume params work).
int ret(int value) {return value;}

int main() {
	// -32768 + -128 + 0 + 127 + 255 + 32767 + -1 = 252
	return ret(-32768) + ret(-128) + ret(0) + ret(127) + ret(255) + ret(32767) + ret(65535) + -251;
}