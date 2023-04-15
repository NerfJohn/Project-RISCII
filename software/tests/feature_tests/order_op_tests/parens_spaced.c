// Check parentheses can handle odd spacing/whitespace.
int main() {
	
	int act1;
	int act2;
	
	act1 = (	 	   (  (
	
(~(    0xfff0    	  	))
 & 0x3
		)	 ^ 0x1 


)		| 0x9);

	act2 = ~(				0x8888&(0x3^(0x1|0x9))								);
	
	
	return act1 + act2 - 1;
}