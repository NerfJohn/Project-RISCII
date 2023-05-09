// Check unused global var causes a warning (error for -Werror).

// WARN: Unused.
unsigned char global;

int main() {return 1;}