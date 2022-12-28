; Check JAL properly uses offset of -256.
LBI r0 1
JAL r7 -256
.blank 32639
LBI r0 0
STR r0 r1 0
HLT