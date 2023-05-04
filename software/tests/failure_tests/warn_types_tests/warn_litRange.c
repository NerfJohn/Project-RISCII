// Check warning due to literal out-of-range prevents compilation (w/ -Werror).

int main () {
	// WARN: 1,000,000 out of int/uint range.
	return 1000000;
}