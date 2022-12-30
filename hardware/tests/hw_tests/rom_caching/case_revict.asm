; "case" tests: Check general cases/states that can result from cache.
; Checks behavior of a cache read evict- generally worset performance.
BRC -nzp 0
LBI r0 1
STR r0 r1 0
BRC -nzp 254

.blank 125

; Read Evict for address 0x100 (conflicts with 0x0 in cache).
NOP
NOP
NOP
HLT