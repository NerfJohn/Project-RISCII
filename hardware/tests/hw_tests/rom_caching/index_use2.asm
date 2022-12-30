; "index_use" tests: Check multiple indices can be loaded/hit in cache.
; Checks hitting between 2 indices gets better performance.
LBI r0 1
STR r0 r1 0
LBI r2 16
JPR r2 0

NOP
NOP
NOP
NOP

BRC -nzp 14
BRC -nzp 14
HLT
NOP

NOP
NOP
NOP
NOP

BRC -nzp -16
BRC -nzp -16
NOP
NOP