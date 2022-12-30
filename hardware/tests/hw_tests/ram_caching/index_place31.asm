; "index_place" tests: Check limits on having multiple indices loaded.
; Checks indices 0 and 31 can be loaded at the same time- generally good performance.
LBI r0 1
STR r0 r1 0
LBI r7 0
LBI r7 -s 248

LDR r6 r6 0
LDR r6 r7 0
LDR r6 r6 0
LDR r6 r7 0