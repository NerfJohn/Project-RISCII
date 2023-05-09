// Check unused function before main causes a warning (error for -Werror).

// WARN: Unused.
int foo() {return 1;}

int main() {return 1;}