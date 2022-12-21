; Check shift works for operand of 1 .
LBI r0 5
LBI r7 1
SHL r0 r0 r7
SUB r0 r0 11
STR r0 r1 0
HLT