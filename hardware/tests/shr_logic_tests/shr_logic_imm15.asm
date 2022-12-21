; Check shift works for immediate of 15.
LBI r0 -1
SHR r0 r0 15
STR r0 r1 0
HLT