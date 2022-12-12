; Check if a shift LBI doesn't let a positive byte sign extend over a negative byte.
LBI r7 -1
LBI r7 -s 0
BRC -.zp 2
LBI r0 1
STR r0 r1 0
HLT