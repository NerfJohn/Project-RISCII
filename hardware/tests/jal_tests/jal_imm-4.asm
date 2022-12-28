; Check JAL properly uses offset of -4.
LBI r0 1
STR r0 r1 0
BRC -nzp 2
HLT
JAL r7 -4
LBI r0 0
STR r0 r1 0
HLT