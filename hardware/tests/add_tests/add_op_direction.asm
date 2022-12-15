; Check ADD operation is directionless (ie A + B == B + A).
LBI r7 2
LBI r6 3
ADD r5 r6 r7
ADD r4 r7 r6
SUB r0 r4 r5
BRC -n.p 4
LBI r0 1
STR r0 r1 0
HLT