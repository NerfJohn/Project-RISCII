; Check if a shift LBI set -32768 is correct.
LBI r7 128
LBI r7 -s 0
LBI r6 127
LBI r6 -s 255
SUB r0 r6 r7
STR r0 r1 0
HLT