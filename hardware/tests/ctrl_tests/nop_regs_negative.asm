; Check registers set to negative numbers aren't changed when NOP is run.
LBI r0 -2
LBI r1 -3
LBI r2 -4
LBI r3 -5
LBI r4 -6
LBI r5 -7
LBI r6 -8
LBI r7 -9
NOP
ADD r0 r0 0
BRC -.zp 32
ADD r1 r1 0
BRC -.zp 28
ADD r2 r2 0
BRC -.zp 24
ADD r3 r3 0
BRC -.zp 20
ADD r4 r4 0
BRC -.zp 16
ADD r5 r5 0
BRC -.zp 12
ADD r6 r6 0
BRC -.zp 8
ADD r7 r7 0
BRC -.zp 4
LBI r0 1
LBI r1 0
STR r0 r1 0
HLT