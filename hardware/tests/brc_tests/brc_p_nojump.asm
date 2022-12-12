; Check ..p doesn't jump for a non-positive number
LBI r7 -1
BRC -..p 2
LBI r0 1
STR r0 r1 0
HLT