; "pback" tests: Check pass back circuit (ALU feedback loop) improves performance.
; Check pass back fails for SR2- should have bad performance.
LDR r0 r7 0
LBI r1 3
LBI r2 4
LBI r3 0

ADD r3 r1 r2
ADD r3 r1 r3
NOP
NOP

SUB r4 r3 10
BRC -n.p 4
LBI r0 1
STR r0 r7 0

HLT