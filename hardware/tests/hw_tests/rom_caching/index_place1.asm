; "index_place" tests: Check limits on having multiple indices loaded.
; Checks indices 0 and 1 can be loaded at the same time- generally good performance.
BRC -nzp 6
BRC -nzp 6
BRC -nzp 10
NOP

BRC -nzp -8
BRC -nzp -8
NOP
NOP

LBI r0 1
STR r0 r1 0
HLT