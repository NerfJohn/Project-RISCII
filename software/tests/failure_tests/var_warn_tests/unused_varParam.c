// Check unused parameter causes a warning (error for -Werror).

// WARN: unused param 'p1'.
int foo(int p1) {return 1;}

int main() {foo(3); return 1;}