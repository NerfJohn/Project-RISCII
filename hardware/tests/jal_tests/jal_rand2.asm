; Checking JAL's correctness via random numbers.
BRC -nzp 50
JPR r3 0
.blank 24
LBI r0 1
JAL r3 -54
STR r0 r1 0
HLT
