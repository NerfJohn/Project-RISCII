; Check NOP doesn't affect random memory address.
LBI r0 -1
LBI r1 126 
STR r0 r1 0
NOP
LDR r2 r1 0
ADD r2 r2 0
BRC -.zp 4
LBI r0 1
STR r0 r7 0
HLT