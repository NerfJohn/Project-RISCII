; Check if shift can clear the register for positive number.
LBI r0 127
LBI r0 -s 255
SHR r0 r0 31
SHR r0 r0 17
ADD r0 r0 1
STR r0 r1 0
HLT