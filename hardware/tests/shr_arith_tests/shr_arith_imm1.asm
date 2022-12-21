; Check shift works for immediate of 1 .
LBI r0 -2
SHR r0 r0 17
SUB r0 r0 0
STR r0 r1 0
HLT