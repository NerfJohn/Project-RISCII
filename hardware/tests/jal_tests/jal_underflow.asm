; Check JAL handles underflow correctly.
ADD r0 r0 0
JAL r7 -10
.blank 32763
LBI r0 1
STR r0 r1 0
HLT