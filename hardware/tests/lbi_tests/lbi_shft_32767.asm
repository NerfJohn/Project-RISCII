; Check if a shift LBI set 32767 is correct.
LBI r7 127
LBI r7 -s 255
LBI r6 127
LBI r6 -s 254
SUB r0 r6 r7
STR r0 r1 0
HLT