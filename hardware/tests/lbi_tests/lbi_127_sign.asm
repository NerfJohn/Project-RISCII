; Check LBI sets 127 correctly by sign.
LBI r7 127
BRC -nz. 2
LBI r0 1
STR r0 r1 0
HLT