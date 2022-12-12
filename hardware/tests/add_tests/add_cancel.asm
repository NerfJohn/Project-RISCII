; Check adding opposite numbers cancel out (ie A + -A = 0).
LBI r7 7
LBI r6 -7
ADD r0 r6 r7
ADD r0 r0 1
STR r0 r1 0
HLT