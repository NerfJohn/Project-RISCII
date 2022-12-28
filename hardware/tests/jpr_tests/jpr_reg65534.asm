; Check JPR works for register with 65534.
LBI r0 1
LBI r7 255
LBI r7 -s 254
JPR r7 14
LBI r0 0
LBI r0 0
STR r0 r1 0
HLT