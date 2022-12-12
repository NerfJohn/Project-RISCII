; Check subtract resulting in sign overflow still calculates "correct" value.
LBI r7 32767
LBI r6 -32768
SUB r0 r6 r7
SUB r0 r0 0
STR r0 r1 0
HLT