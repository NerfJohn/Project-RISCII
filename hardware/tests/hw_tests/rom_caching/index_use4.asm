; "index_use" tests: Check multiple indices can be loaded/hit in cache.
; Checks hitting between 4 indices gets the worst performance.
LBI r0 1
STR r0 r1 0
LBI r2 16
JPR r2 0

NOP
NOP
NOP
NOP

BRC -nzp 14
HLT
NOP
NOP

BRC -nzp -8
NOP
NOP
NOP

BRC -nzp 6
NOP
NOP
NOP

BRC -nzp -18
NOP
NOP
NOP