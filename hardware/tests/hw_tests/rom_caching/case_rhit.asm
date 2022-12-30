; "case" tests: Check general cases/states that can result from cache.
; Checks behavior of a cache read hit- generally good performance.
BRC -nzp 12
LBI r0 1
STR r0 r1 0
NOP

; Read Hit for address 0x8.
NOP
HLT
NOP
BRC -nzp -14