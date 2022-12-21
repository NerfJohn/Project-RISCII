; Check shift works for operand of 15.
LBI r0 128
LBI r0 -s 0
LBI r6 31
SHR r0 r0 r6
ADD r0 r0 2
STR r0 r1 0
HLT