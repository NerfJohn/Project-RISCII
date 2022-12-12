; Check adding 0 via operand works correctly.
LBI r0 1
ADD r0 r0 r1
STR r0 r1 0
HLT