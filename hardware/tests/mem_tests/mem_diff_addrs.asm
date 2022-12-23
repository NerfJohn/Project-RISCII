; Checks different addresses access different 2-byte slots.
LBI r7 3
LBI r6 4
STR r7 r7 -3
STR r6 r6 -2
LDR r5 r5 0
LDR r4 r4 2
SUB r0 r5 r4
STR r0 r1 0
HLT
