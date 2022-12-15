; Check XOR works as expected for a 'negative' immediate.
LBI r7 -1
XOR r7 r7 16
BRC -nz. 4
LBI r0 1
STR r0 r1 0
HLT