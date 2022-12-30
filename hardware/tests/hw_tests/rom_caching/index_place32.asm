; "index_place" tests: Check limits on having multiple indices loaded.
; Checks indices 0 and 32 CAN'T be loaded at the same time- generally bad performance.
BRC -nzp 254
BRC -nzp 254
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

.blank 122

BRC -nzp -256
BRC -nzp -256
NOP
NOP