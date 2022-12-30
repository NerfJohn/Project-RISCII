; "pback" tests: Check pass back circuit (ALU feedback loop) improves performance.
; Ideal case of pass back- should have good performance.
LDR r0 r7 0
LBI r1 3
LBI r2 4
LBI r3 0

ADD r3 r1 r2
ADD r3 r3 r1
NOP
NOP

SUB r4 r3 10
BRC -n.p 4
LBI r0 1
STR r0 r7 0

HLT