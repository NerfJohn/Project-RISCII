; "index_use" tests: Check multiple indices can be loaded/hit in cache.
; Checks hitting between 3 indices gets worse performance.
LBI r0 1
STR r0 r1 0
LBI r7 32
NOP

LDR r6 r7 0
LDR r6 r7 16
LDR r6 r7 24
LDR r6 r7 0