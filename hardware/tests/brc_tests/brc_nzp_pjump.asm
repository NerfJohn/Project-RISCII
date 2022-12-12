; Check nzp jumps for a positive number.
LBI r0 1
LBI r7 1
BRC -nzp 2
LBI r0 0
STR r0 r1 0
HLT