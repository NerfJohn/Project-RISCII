; Check JPR works for offset of -32.
LBI r0 1
LBI r7 44
JPR r7 -32
LBI r0 0
LBI r0 0
LBI r0 0
STR r0 r1 0
HLT