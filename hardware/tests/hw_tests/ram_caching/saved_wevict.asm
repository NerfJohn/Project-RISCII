; "saved" tests: Check evicted blocks are actually saved in RAM.
; Checks a write evict will properly save the evicted block.
LBI r7 1
LBI r7 -s 0
LBI r6 6
STR r6 r1 0

; Evict via write (and back).
STR r7 r7 0
LDR r0 r1 0
NOP
NOP

; Check its the same.
SUB r0 r0 6
BRC -n.p 4
LBI r0 1
STR r0 r1 0

HLT