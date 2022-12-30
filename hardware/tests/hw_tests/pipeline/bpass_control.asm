; "bpass" tests: Check bypass circuit (between writeback and reg. file) improves performance.
; Control test to compare bypass tests against.
LDR r0 r7 0
LBI r1 3
LBI r2 4
LBI r3 0

NOP
NOP
NOP
NOP

SUB r4 r3 0
BRC -n.p 4
LBI r0 1
STR r0 r7 0

HLT