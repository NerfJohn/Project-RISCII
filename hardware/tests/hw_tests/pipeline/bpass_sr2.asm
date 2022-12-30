; "bpass" tests: Check bypass circuit (between writeback and reg. file) improves performance.
; Checks if bypass can handle SR2 bypass- should trigger/have good performance.
LDR r0 r7 0
LBI r1 3
LBI r2 4
LBI r3 0

ADD r3 r1 r2
NOP
NOP
ADD r3 r2 r3

SUB r4 r3 11
BRC -n.p 4
LBI r0 1
STR r0 r7 0

HLT