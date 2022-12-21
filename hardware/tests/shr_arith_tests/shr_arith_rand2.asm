; Check shift works for randon values.
LBI r0 -15
LBI r7 26
LBI r6 0
SHR r0 r0 r7
SUB r0 r0 r6
STR r0 r1 0
HLT