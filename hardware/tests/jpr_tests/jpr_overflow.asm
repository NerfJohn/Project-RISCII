; Check JPR still works for overflowed sum.
LBI r0 1
LBI r7 255
LBI r7 -s 254
JPR r7 12
LBI r0 0
STR r0 r1 0
HLT