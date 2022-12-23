; Checks different expressions with the same sum use the same address (ie MEM[A+B] = MEM[B+A]).
LBI r7 2
LBI r6 1
LBI r5 2
LBI r4 4
STR r7 r5 4
STR r6 r4 2
LDR r3 r5 4
LDR r2 r4 2
SUB r0 r2 r3
BRC -n.p 4
LBI r0 1
STR r0 r1 0
HLT