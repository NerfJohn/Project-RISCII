; Check shift works for randon values.
LBI r0 192
LBI r0 -s 0
LBI r7 28
LBI r6 5
SHR r0 r0 r7
ADD r0 r0 r6
STR r0 r1 0
HLT