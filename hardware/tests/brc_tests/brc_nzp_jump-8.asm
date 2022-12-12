; Check brc can handle negative jumps.
BRC -nzp 6
LBI r0 1
STR r0 r1 0
HLT
BRC -nzp -8