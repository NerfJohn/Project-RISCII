; Check shift works for immediate of 1 .
LBI r0 5
SHL r0 r0 1
SUB r0 r0 11
STR r0 r1 0
HLT