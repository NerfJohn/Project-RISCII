; Check ORR works as expected for an all 0s operand (ie pass).
LBI r0 -1
ORR r0 r0 r1
ADD r0 r0 2
STR r0 r1 0
HLT