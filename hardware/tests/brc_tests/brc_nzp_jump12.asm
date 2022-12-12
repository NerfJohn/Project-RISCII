; Check accuracy of jumping (and jump besides 2)
LBI r0 1
BRC -nzp 12
HLT
HLT
HLT
HLT
HLT
HLT
STR r0 r1 0
HLT