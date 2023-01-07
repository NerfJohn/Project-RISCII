; "case" tests: Check general cases/states that can result from cache.
; Checks behavior of a cache write hit- generally good performance.
LBI r0 1
STR r0 r1 0
LBI r6 1
LBI r7 0

; Cache write hit for address 0x0.
STR r6 r7 0
LDR r0 r1 0