; Check JAL properly puts link in register 7.
JAL r7 0
ADD r7 r7 0
BRC -nz. 4
LBI r0 1
STR r0 r1 0
HLT