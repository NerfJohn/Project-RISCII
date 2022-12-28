; Check JLR handles underflow correctly.
JLR r6 r7 -10
.blank 32762
LBI r0 1
STR r0 r1 0
HLT
HLT
HLT