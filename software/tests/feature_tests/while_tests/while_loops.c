// Check while statements act as loops until broken out of.
int main() {
	int i;
	
	i = 0;
	while (i < 3) {
		i = i + 1;
	}
	
	return i - 2;
}