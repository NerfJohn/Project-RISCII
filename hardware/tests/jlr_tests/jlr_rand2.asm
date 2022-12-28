; Checking JLR's correctness via random numbers.
BRC -nzp 50
JPR r6 0
.blank 24
LBI r0 1
LBI r4 6
JLR r6 r4 -4
STR r0 r1 0
HLT
