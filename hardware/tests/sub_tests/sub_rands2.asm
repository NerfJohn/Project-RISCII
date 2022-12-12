; Check subtract with general/random values.
LBI r7 -30
LBI r6 12
LBI r0 -43
SUB r5 r6 r7
SUB r0 r0 r5
STR r0 r1 0
HLT