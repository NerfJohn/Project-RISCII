; Check LBI sets 0 correctly by subtraction.
LBI r7 0
LBI r6 -1
SUB r0 r6 r7
STR r0 r1 0
HLT