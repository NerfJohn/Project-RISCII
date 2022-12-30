; "bpass" tests: Check bypass circuit (between writeback and reg. file) improves performance.
; Checks result of offset ordering- should not trigger/have bad performance
LDR r0 r7 0
LBI r1 3
LBI r2 4
LBI r3 0

LDR r1 r7 0
NOP
ADD r3 r1 r2
NOP

SUB r4 r3 4
BRC -n.p 4
LBI r0 1
STR r0 r7 0

HLT