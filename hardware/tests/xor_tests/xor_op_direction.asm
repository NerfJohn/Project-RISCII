; Check XOR operation is directionless (ie A ^ B == B ^ A).
LBI r7 19
LBI r6 7
XOR r5 r6 r7
XOR r4 r7 r6
SUB r0 r4 r5
ADD r0 r0 1
STR r0 r1 0
HLT