; Check sub respects identity property (ie A - A = 0).
LBI r7 255
SUB r6 r7 r7
BRC -.z. 2
HLT
LBI r0 1
STR r0 r1 0
HLT