// Check unused var after main causes a warning (error for -Werror).

int main() {return 1;}

// WARN: Unused.
unsigned char unused;