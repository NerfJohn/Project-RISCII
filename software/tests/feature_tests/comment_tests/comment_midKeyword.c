// Check comment can "interrupt" keywords.
int main() {
	unsigned // Interrupting comment!
	int 	// Twice!
	i;
	
	i = 1;
	
	return i;
}