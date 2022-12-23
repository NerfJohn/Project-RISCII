; Check storing to one address twice saves the most recent store.
LBI r7 7
LBI r6 1
STR r7 r1 2
STR r6 r1 2
LDR r0 r1 2
STR r0 r1 0
HLT
