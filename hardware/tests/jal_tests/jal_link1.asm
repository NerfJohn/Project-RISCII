; Check JAL properly puts link in register 1.
JAL r1 0
ADD r1 r1 0
BRC -nz. 4
LBI r0 1
STR r0 r7 0
HLT