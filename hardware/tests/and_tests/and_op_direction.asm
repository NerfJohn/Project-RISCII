; Check AND operation is directionless (ie A & B == B & A).
LBI r7 7
LBI r6 5
AND r5 r6 r7
AND r4 r7 r6
SUB r0 r4 r5
ADD r0 r0 1
STR r0 r1 0
HLT