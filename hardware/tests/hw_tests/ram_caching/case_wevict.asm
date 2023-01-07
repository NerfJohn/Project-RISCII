; "case" tests: Check general cases/states that can result from cache.
; Checks behavior of a cache write evict- generally worst performance.
LBI r0 1
STR r0 r1 0
LBI r7 1
LBI r7 -s 0

; Cache write evict for address 0x100 (evicts address 0x0-0x3).
STR r6 r7 0
LDR r0 r1 0