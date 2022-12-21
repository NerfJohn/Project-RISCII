; Check shift works for randon values.
LBI r0 34
LBI r7 18
LBI r6 9
SHR r0 r0 r7
SUB r0 r0 r6
STR r0 r1 0
HLT