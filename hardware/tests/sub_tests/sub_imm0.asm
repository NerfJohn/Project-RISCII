; Check subtracting with immediate 0 works correctly (ie arithmetic invert).
LBI r0 -1
SUB r0 r0 0
STR r0 r1 0
HLT