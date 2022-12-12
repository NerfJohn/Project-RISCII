; Check LBI sets -1 correctly by sign.
LBI r7 -1
BRC -.zp 2
LBI r0 1
STR r0 r1 0
HLT