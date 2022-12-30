; "pback" tests: Check pass back circuit (ALU feedback loop) improves performance.
; Check pass back doesn't work for "offset" instructions- should have bad performance.
LDR r0 r7 0
LBI r1 3
LBI r2 4
LBI r3 0

ADD r3 r1 r2
NOP
ADD r3 r3 r2
NOP

SUB r4 r3 11
BRC -n.p 4
LBI r0 1
STR r0 r7 0

HLT