; Check store value of 2 works correctly for LDR/STR.
LBI r7 2
STR r7 r1 0
LDR r6 r1 0
SUB r6 r6 2
BRC -n.p 4
LBI r0 1
STR r0 r1 0
HLT