; Check shift works for operand of 15.
LBI r0 -1
LBI r6 15
SHR r0 r0 r6
STR r0 r1 0
HLT