; Check ORR works as expected for a mixed 1s/0s immediate (ie pass and sets).
LBI r0 85
LBI r7 -86
ORR r0 r0 7
ADD r0 r0 r7
STR r0 r1 0
HLT