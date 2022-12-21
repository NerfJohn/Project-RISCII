; Check if shift can set the register via arithmetic shifting with 1.
LBI r0 128
LBI r0 -s 0
SHR r0 r0 31
SHR r0 r0 17
ADD r0 r0 2
STR r0 r1 0
HLT