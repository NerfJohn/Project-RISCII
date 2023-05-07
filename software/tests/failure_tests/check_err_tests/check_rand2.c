// Check errors during "check phases" using random(ish) test case.

// ERR: foobar in function refers to variable, NOT function.
int foobar(int foobar) {return foobar(4);}

int main() {return foobar(5);}