// Check warning due to uninitialized var prevents compilation (w/ -Werror).

int main() {
	// WARN: Uninitialized var.
	int uninit;
	
	return uninit;
}