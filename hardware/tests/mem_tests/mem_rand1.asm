; Check LDR/STR with random values.
LBI r7 24
LBI r6 16
LBI r5 -31
LBI r4 15
STR r6 r7 12
STR r4 r5 -1
LDR r1 r7 12
LDR r0 r5 -1
SUB r0 r0 r1
LBI r1 0
STR r0 r1 0
HLT 