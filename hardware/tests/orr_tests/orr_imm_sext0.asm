; Check ORR works as expected for a 'positive' immediate.
LBI r7 0
ORR r7 r7 15
BRC -nz. 4
LBI r0 1
STR r0 r1 0
HLT