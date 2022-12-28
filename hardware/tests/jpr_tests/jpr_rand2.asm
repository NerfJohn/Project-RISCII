; Check JPR works for random values.
LBI r0 1
LBI r7 -1
BRC -nzp 4
STR r0 r1 0
HLT
JPR r7 7
LBI r0 0
HLT