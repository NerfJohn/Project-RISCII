; Check LDR/STR with random values.
LBI r7 -100
LBI r6 -51
LBI r5 100
LBI r4 -52
STR r6 r7 50
STR r4 r5 -20
LDR r1 r7 50
LDR r0 r5 -20
SUB r0 r0 r1
LBI r1 0
STR r0 r1 0
HLT 