; Check adding with immediate 15 works correctly.
LBI r0 -14
ADD r0 r0 15
STR r0 r1 0
HLT