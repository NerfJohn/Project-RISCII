; Check NOP doesn't affect MEM[0].
LBI r0 -1
STR r0 r1 0
NOP
LDR r2 r1 0
ADD r2 r2 0
BRC -.zp 4
LBI r0 1
STR r0 r1 0
HLT