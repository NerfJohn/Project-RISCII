// Check no return in function causes warning (error for _Werror).

// WARN: no obvious return.
int noReturn(){}

int main() {
	noReturn();
	return 1;
}