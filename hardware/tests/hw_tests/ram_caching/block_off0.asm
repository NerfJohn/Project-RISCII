; "block_off" tests: Check block size = 4 and aligned.
; Checks 4 aligned accesses get better performance.
LBI r0 1
STR r0 r1 0
LBI r7 16
NOP

LDR r6 r7 0
LDR r6 r7 2
LDR r6 r7 4
LDR r6 r7 6