; Check JLR properly puts link in register 1.
JLR r1 r6 2
ADD r1 r1 0
BRC -nz. 4
LBI r0 1
STR r0 r6 0
HLT