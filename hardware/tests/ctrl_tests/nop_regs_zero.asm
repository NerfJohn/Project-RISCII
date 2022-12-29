; Check registers set to zero aren't changed when NOP is run.
NOP
ADD r0 r0 0
BRC -n.p 32
ADD r1 r1 0
BRC -n.p 28
ADD r2 r2 0
BRC -n.p 24
ADD r3 r3 0
BRC -n.p 20
ADD r4 r4 0
BRC -n.p 16
ADD r5 r5 0
BRC -n.p 12
ADD r6 r6 0
BRC -n.p 8
ADD r7 r7 0
BRC -n.p 4
LBI r0 1
STR r0 r1 0
HLT