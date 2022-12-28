; Check linked PC value is as expected (ie check the value).
LBI r0 1
JAL r7 2
HLT
ADD r7 r7 -4
BRC -n.p 2
STR r0 r1 0
HLT