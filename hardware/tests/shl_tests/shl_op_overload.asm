; Check shift with value over 15 gets truncated.
LBI r0 5
LBI r7 17
SHL r0 r0 r7
SUB r0 r0 11
STR r0 r1 0
HLT