; Check .z. jumps for zero
LBI r0 1
LBI r7 0
BRC -.z. 2
LBI r0 0
STR r0 r1 0
HLT