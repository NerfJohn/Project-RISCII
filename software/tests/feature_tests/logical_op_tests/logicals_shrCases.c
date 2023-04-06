// Check correct shift right cases are used.
int main() {
  // Stand-in variables for variable cases.
  int int_t;
  unsigned int uint_t;
  char char_t;
  unsigned char uchar_t;
  int_t = -1;
  uint_t = -1;
  char_t = -1;
  uchar_t = -1;
  
  int shr0;
  int shr1;
  int shr2;
  int shr3;
  int shr4;
  int shr5;
  int shr6;
  int shr7;
  
  // Decide by literal.
  shr0 = 0xffff >> 1;
  shr1 = -1 >> 1;
  shr2 = 1 >> 1;
  shr7 = '\n' >> 1;
  
  // Decide by var.
  shr3 = int_t >> 1;
  shr4 = uint_t >> 1;
  shr5 = char_t >> 1;
  shr6 = uchar_t >> 1;
  
  // Return sum to check value (32767 + -1 + 0 + 5 + -1 + 32767 + -1 + 127 = 127 + 65536)
  return shr0 + shr1 + shr2 + shr3 + shr4 + shr5 + shr6 + -126; 
}
