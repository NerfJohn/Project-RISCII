; Check AND works as expected for an all 0s operand (ie clear).
LBI r0 -1
AND r0 r0 r1
ADD r0 r0 1
STR r0 r1 0
HLT