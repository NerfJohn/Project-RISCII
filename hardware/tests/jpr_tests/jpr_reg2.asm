; Check JPR works for register with 2.
LBI r0 1
LBI r7 2
JPR r7 8
LBI r0 0
LBI r0 0
STR r0 r1 0
HLT