; Check XOR works as expected for an all 1s operand (ie toggle).
LBI r0 -1
LBI r7 2
XOR r0 r0 r7
ADD r0 r0 4
STR r0 r1 0
HLT