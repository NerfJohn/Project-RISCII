; Check JPR still works for underflowed sum.
LBI r0 1
LBI r7 0
JPR r7 -32
.blank 32748
LBI r0 0
STR r0 r1 0
HLT