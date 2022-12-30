; "case" tests: Check general cases/states that can result from cache.
; Checks behavior of a cache read miss- generally worse performance.
LBI r0 1
STR r0 r1 0
LBI r7 0
LBI r7 8

; Cache read miss for address 0x8.
LDR r6 r7 0