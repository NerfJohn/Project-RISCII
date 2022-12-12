; Check ... doesn't jump for zero.
LBI r7 0
BRC -... 2
LBI r0 1
STR r0 r1 0
HLT