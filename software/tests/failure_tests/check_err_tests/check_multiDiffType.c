// Check multiply defined names (even with different types) cause an error.

char var;

// ERR: Multiply declared name.
int var() {return 1;}

int main() {return var();}