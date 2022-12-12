; Check if a shift LBI doesn't let a negative byte sign extend over a positive byte.
LBI r7 0
LBI r7 -s -1
BRC -nz. 2
LBI r0 1
STR r0 r1 0
HLT