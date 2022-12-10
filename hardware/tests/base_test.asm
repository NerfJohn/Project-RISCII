; Base test case: Sets address 0x0 to 1 as quickly as possible
; A test is considered successful if the first word in memory is 1
LBI r0 1
STR r0 r1 0
HLT