// Check reused names in different scopes are okay for functions/parameters.
int name1(char param1) {}

char name2(int name1, char param1) {}

unsigned char param1(int main, char name2, char param1){}

int main() {return 1;}