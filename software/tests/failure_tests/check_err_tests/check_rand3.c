// Check errors during "check phases" using random(ish) test case.

int main() {return 1;}

// ERR: Multiply defined function (happens to be main).
int main() {return 1;}