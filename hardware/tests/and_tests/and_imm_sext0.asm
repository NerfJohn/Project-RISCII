; Check AND works as expected for a 'positive' immediate.
LBI r7 -1
AND r7 r7 15
BRC -nz. 4
LBI r0 1
STR r0 r1 0
HLT