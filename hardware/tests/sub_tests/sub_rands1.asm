; Check subtract with general/random values.
LBI r7 15
LBI r6 20
LBI r0 -6
SUB r5 r6 r7
SUB r0 r0 r5
STR r0 r1 0
HLT