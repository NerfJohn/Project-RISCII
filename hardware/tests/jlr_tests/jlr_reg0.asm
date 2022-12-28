; Check JLR works for register of 0.
JLR r6 r0 10
HLT
LBI r0 1
STR r0 r1 0
HLT
BRC -nzp -8