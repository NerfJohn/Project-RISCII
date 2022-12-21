; Check shift works for randon values.
LBI r0 -2
LBI r7 4
LBI r6 -31
SHL r0 r0 r7
SUB r0 r0 r6
STR r0 r1 0
HLT