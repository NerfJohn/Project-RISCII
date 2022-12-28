; Checking JAL's correctness via random numbers.
ADD r0 r0 0
ADD r0 r0 0
ADD r0 r0 0
ADD r0 r0 0
JAL r5 40
STR r0 r1 0
.blank 19
LBI r0 1
JPR r5 0
HLT