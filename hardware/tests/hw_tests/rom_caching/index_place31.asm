; "index_place" tests: Check limits on having multiple indices loaded.
; Checks indices 0 and 31 can be loaded at the same time- generally good performance.
BRC -nzp 246
BRC -nzp 246
BRC -nzp 10
NOP

NOP
NOP
NOP
NOP

LBI r0 1
STR r0 r1 0
HLT
NOP

.blank 112

BRC -nzp -248
BRC -nzp -248
NOP
NOP