; Check JLR properly puts link in register 0.
JLR r0 r6 2
ADD r0 r0 0
BRC -nz. 4
LBI r0 1
STR r0 r1 0
HLT