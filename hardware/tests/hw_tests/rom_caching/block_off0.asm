; "block_off" tests: Check block size = 4 and aligned.
; Checks 4 aligned accesses get better performance.
LBI r0 1
STR r0 r1 0
LBI r2 24
JPR r2 0

NOP
NOP
NOP
NOP

NOP
NOP
NOP
NOP

; Jumps to line below.
NOP
NOP
NOP
HLT