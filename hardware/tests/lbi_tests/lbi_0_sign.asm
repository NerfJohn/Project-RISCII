; Check LBI sets 0 correctly by sign.
LBI r7 0
BRC -n.p 2
LBI r0 1
STR r0 r1 0
HLT