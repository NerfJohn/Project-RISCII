// Check various "unused functions" are handled correctly (deleted by -O1 flag).

int unusedFoo(char p1) {return p1;} // OPT: Delete

int main() {return 1;}

unsigned char unusedBar() {return 2;} // OPT: Delete