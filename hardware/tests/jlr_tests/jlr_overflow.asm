; Check JLR handles overflow correctly.
LBI r0 1
LBI r7 -4
JPR r7 0
HLT
STR r0 r1 0
HLT
.blank 32760
JLR r6 r7 12
HLT