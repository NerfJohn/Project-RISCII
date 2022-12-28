; Check linked PC value can be used correctly.
JAL r7 4
STR r0 r1 0
HLT
LBI r0 1
JPR r7 0
