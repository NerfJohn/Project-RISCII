; Check ... doesn't jump for a negative number.
LBI r7 -1
BRC -... 2
LBI r0 1
STR r0 r1 0
HLT