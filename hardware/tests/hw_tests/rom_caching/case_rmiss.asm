; "case" tests: Check general cases/states that can result from cache.
; Checks behavior of a cache read miss- generally worse performance.
BRC -nzp 0
BRC -nzp 0
LBI r0 1
STR r0 r1 0

; Read miss for address 0x8.
NOP
HLT
NOP
NOP