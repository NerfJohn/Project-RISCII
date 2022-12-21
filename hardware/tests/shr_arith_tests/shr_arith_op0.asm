; Check shift works for operand of 0 (ie no op).
LBI r0 -1
LBI r7 16
SHR r0 r0 r7
ADD r0 r0 2
STR r0 r1 0
HLT