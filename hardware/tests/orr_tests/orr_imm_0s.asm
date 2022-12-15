; Check ORR works as expected for an all 0s immediate (ie pass).
LBI r0 0
ORR r0 r0 0
ADD r0 r0 1
STR r0 r1 0
HLT