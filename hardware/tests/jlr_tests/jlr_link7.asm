; Check JLR properly puts link in register 7.
JLR r7 r6 2
ADD r7 r7 0
BRC -nz. 4
LBI r0 1
STR r0 r1 0
HLT