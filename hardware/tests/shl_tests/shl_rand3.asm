; Check shift works for randon values.
LBI r0 50
LBI r7 12
LBI r6 32
LBI r6 -s 4
SHL r0 r0 r7
SUB r0 r0 r6
STR r0 r1 0
HLT