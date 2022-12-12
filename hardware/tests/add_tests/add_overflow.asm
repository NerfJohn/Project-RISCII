; Check add resulting in unsigned overflow works correctly (-2 == 65534).
LBI r6 -2
LBI r7 3
ADD r0 r6 r7
STR r0 r1 0
HLT