; Check shift works for operand of 15 (1 << 15 = 0x80, -128 w/ 1 = 0x81).
LBI r0 1
LBI r7 -128
LBI r7 -s 1
LBI r6 15
SHL r0 r0 r6
SUB r0 r0 r7
STR r0 r1 0
HLT