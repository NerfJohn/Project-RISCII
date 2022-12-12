; Check LBI sets -128 correctly by subtraction.
LBI r7 -128
LBI r6 -128
LBI r5 -1
SUB r0 r6 r7
SUB r0 r5 r0
STR r0 r1 0
HLT