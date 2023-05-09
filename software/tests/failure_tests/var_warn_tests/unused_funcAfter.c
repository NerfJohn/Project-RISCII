// Check unused function after main causes a warning (error for -Werror).

int main() {return 1;}

// WARN: Unused.
int foo() {return 1;}