; Check n.. doesn't jump for a non-negative number
LBI r7 1
BRC -n.. 2
LBI r0 1
STR r0 r1 0
HLT