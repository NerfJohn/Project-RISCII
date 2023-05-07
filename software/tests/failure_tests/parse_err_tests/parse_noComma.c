// Check missing comma causes an error.

// ERR: Missing comma.
int foo(int p1 int p2) {return 1;}

int main() {return foo(1,1);}