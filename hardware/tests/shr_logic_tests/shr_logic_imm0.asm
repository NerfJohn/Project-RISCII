; Check shift works for immediate of 0 (ie no op).
LBI r0 1
SHR r0 r0 0
STR r0 r1 0
HLT