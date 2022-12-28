; Check JLR works for register of 2.
LBI r0 2
JLR r6 r0 4
HLT
LBI r0 1
STR r0 r1 0
HLT