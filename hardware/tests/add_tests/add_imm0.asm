; Check adding with immediate 0 works correctly (ie no op).
LBI r0 1
ADD r0 r0 0
STR r0 r1 0
HLT