; Check if a shift LBI set 0 is correct.
LBI r7 0
LBI r7 -s 0
LBI r6 -1
SUB r0 r6 r7
STR r0 r1 0
HLT