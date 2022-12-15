; Check XOR works as expected for "random" values.
LBI r7 14
LBI r6 22
LBI r5 -23
XOR r0 r6 r7
ADD r0 r0 r5
STR r0 r1 0
HLT