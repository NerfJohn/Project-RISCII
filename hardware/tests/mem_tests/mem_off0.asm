; Check offset of 0 works correctly for LDR/STR.
LBI r7 3
STR r7 r1 0
LDR r6 r1 0
SUB r6 r6 3
BRC -n.p 2
LBI r0 1
STR r0 r1 0
HLT