; Check adding works with "random" numbers.
LBI r7 -3
LBI r6 16
LBI r5 12
ADD r0 r6 r7
SUB r0 r5 r0
STR r0 r1 0
HLT