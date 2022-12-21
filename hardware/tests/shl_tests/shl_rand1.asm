; Check shift works for randon values.
LBI r0 17
LBI r7 3
LBI r6 -s 137
SHL r0 r0 r7
SUB r0 r0 r6
STR r0 r1 0
HLT