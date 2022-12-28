; Check JPR works for offset of 0.
LBI r0 1
LBI r7 8
JPR r7 0
LBI r0 0
STR r0 r1 0
HLT