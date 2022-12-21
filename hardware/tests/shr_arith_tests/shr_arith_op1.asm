; Check shift works for operand of 1 .
LBI r0 5
LBI r7 17
SHR r0 r0 r7
SUB r0 r0 3
STR r0 r1 0
HLT