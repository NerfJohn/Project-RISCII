; Check JLR works for register of 65534.
LBI r0 -2
JLR r6 r0 0
.blank 32762
LBI r0 1
STR r0 r1 0
HLT
BRC -nzp -8