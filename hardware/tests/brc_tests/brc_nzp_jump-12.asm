; Check brc can handle accurate, negative jumps.
LBI r0 1
BRC -nzp 16
HLT
HLT
HLT
STR r0 r1 0
HLT
HLT
HLT
HLT
BRC -nzp -12