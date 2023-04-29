// Check comment can "interrupt" keywords.
int main() {
	unsigned // Interrupting comment!
	int 	// Twice!
	i;
	
	i = 0;
	
	return i;
}