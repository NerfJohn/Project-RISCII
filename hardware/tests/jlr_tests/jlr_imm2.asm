; Check JLR works for offset of 2.
LBI r0 1
LBI r7 6
JLR r6 r7 2
LBI r0 0
STR r0 r1 0
HLT