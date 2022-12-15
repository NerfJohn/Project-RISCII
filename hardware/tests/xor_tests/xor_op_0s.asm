; Check XOR works as expected for an all 0s operand (ie pass).
LBI r0 4
XOR r0 r0 r1
ADD r0 r0 -3
STR r0 r1 0
HLT