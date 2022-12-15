; Check ORR works as expected for a 'negative' immediate.
LBI r7 0
ORR r7 r7 16
BRC -.zp 4
LBI r0 1
STR r0 r1 0
HLT