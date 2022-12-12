; Check n.. jumps for a negative number
LBI r0 1
LBI r7 -1
BRC -n.. 2
LBI r0 0
STR r0 r1 0
HLT