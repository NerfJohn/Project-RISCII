; Check ORR works as expected for "random" values.
LBI r7 55
LBI r6 54
LBI r5 -54
ORR r0 r6 r7
ADD r0 r0 r5
STR r0 r1 0
HLT