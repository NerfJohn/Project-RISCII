; "pback" tests: Check pass back circuit (ALU feedback loop) improves performance.
; Control test to compare pass back tests against.
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