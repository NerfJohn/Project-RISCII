; Check NOP doesn't affect MEM[65534] (ie last 2-byte address).
LBI r0 -1
LBI r1 255
LBI r1 -s 255
STR r0 r1 0
NOP
LDR r2 r1 0
ADD r2 r2 0
BRC -.zp 4
LBI r0 1
STR r0 r7 0
HLT