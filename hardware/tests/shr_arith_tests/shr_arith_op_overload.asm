; Check shift with value over 15 gets truncated (33 = 0b1(1)0001.
LBI r0 5
LBI r7 49
SHR r0 r0 r7
SUB r0 r0 3
STR r0 r1 0
HLT