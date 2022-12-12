; Check .z. doesn't jump for a non-zero number
LBI r7 1
BRC -.z. 2
LBI r0 1
STR r0 r1 0
HLT