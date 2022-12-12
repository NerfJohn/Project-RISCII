; Check LBI sets -128 correctly by sign.
LBI r7 -128
BRC -.zp 2
LBI r0 1
STR r0 r1 0
HLT