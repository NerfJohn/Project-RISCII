; Check XOR works as expected for an all 1s immediate (ie toggle- also, effectively bitwise NOT).
LBI r0 1
XOR r0 r0 31
ADD r0 r0 3
STR r0 r1 0
HLT