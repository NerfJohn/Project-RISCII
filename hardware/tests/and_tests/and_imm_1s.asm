; Check AND works as expected for an all 1s immediate (ie pass).
LBI r0 -1
AND r0 r0 31
ADD r0 r0 2
STR r0 r1 0
HLT