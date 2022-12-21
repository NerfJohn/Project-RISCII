; Check if shift can clear the register.
LBI r0 -1
SHR r0 r0 15
SHR r0 r0 1
ADD r0 r0 1
STR r0 r1 0
HLT