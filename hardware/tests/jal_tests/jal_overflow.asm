; Check JAL handles overflow correctly.
LBI r7 -4
JPR r7 0
HLT
LBI r0 1
STR r0 r1 0
HLT
.blank 32760
JAL r7 8
HLT