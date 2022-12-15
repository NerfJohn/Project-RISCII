; Check ORR works as expected for an all 1s immediate (ie set).
LBI r0 0
ORR r0 r0 31
ADD r0 r0 2
STR r0 r1 0
HLT