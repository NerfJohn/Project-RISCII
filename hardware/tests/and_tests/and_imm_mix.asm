; Check AND works as expected for a mixed 1s/0s immediate (ie pass and clears).
LBI r0 85
AND r0 r0 7
ADD r0 r0 -4
STR r0 r1 0
HLT