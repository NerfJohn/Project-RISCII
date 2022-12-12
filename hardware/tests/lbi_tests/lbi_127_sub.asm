; Check LBI sets 127 correctly by subtraction.
LBI r7 127
LBI r6 126
SUB r0 r6 r7
STR r0 r1 0
HLT