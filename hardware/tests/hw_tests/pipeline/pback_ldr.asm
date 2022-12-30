; "pback" tests: Check pass back circuit (ALU feedback loop) improves performance.
; Check LDR is flagged as "false positive" for passing back SR1- should have bad performance.
LDR r0 r7 0
LBI r1 3
LBI r2 4
LBI r3 0

LDR r3 r7 0
ADD r3 r3 r1
NOP
NOP

SUB r4 r3 3
BRC -n.p 4
LBI r0 1
STR r0 r7 0

HLT