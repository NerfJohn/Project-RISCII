; Check AND works as expected for a mixed 1s/0s operand (ie pass and clears).
LBI r0 85
LBI r7 7
AND r0 r0 r7
ADD r0 r0 -4
STR r0 r1 0
HLT