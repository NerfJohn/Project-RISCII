; Check store value of -65535 works correctly for LDR/STR.
LBI r7 255
LBI r7 -s 255
STR r7 r1 0
LDR r6 r1 0
SUB r6 r6 r7
BRC -n.p 4
LBI r0 1
STR r0 r1 0
HLT