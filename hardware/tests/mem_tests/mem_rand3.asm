; Check LDR/STR with random values.
LBI r7 13
LBI r6 127
LBI r5 92
LBI r4 126
STR r6 r7 9
STR r4 r5 4
LDR r1 r7 9
LDR r0 r5 4
SUB r0 r0 r1
LBI r1 0
STR r0 r1 0
HLT 