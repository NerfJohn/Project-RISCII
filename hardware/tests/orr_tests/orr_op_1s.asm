; Check ORR works as expected for an all 1s operand (ie set).
LBI r0 -1
ORR r0 r0 r7
ADD r0 r0 2
STR r0 r1 0
HLT