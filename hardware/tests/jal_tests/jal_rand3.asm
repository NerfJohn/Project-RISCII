; Checking JAL's correctness via random numbers.
JAL r1 -8
LBI r0 1
STR r0 r1 0
.blank 32762
ADD r7 r1 0
LBI r1 0
JPR r7 0
