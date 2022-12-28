; Check JAL properly uses offset of 254.
LBI r0 1
JAL r7 254
.blank 126
LBI r0 0
STR r0 r1 0
HLT