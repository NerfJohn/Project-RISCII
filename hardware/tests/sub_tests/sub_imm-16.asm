; Check subtracting with immediate -16 works correctly.
LBI r0 -17
SUB r0 r0 -16
STR r0 r1 0
HLT