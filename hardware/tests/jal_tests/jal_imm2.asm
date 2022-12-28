; Check JAL properly uses offset of 2.
LBI r0 1
JAL r7 2
LBI r0 0
STR r0 r1 0
HLT