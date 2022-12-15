; Check AND works as expected for "random" values.
LBI r7 24
LBI r6 48
LBI r5 -15
AND r0 r6 r7
ADD r0 r0 r5
STR r0 r1 0
HLT