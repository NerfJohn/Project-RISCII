// Check function without return type causes an error.

// ERR: No return type.
foo() {return 1;}

int main() {return foo();}