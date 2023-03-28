// Check escaped chars are accepted by language.

int main() {
	char tab;
	unsigned char feed;
	int null;
	unsigned int backslash;
	char apost;
	
	tab = '\t';
	feed = '\n';
	null = '\0';
	backslash = '\\';
	apost = '\'';
	
	return tab + feed + null + backslash + apost + -159 + '\n'; // -149 = -159 + (\n=10)
}