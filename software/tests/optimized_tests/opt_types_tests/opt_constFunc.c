// Check "constant function" is implemented correctly (for -O1 flag).

// OPT: Treat calls as literal '1'.
int CONST() {return 1;}

int main() {return CONST();}