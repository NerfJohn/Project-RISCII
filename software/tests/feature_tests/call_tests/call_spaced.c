// Check function calls can handle weird spacing.
int foo(int i) {return 1;}
int bar(unsigned char uc, unsigned int ui, char c) {return 1;}
int barfoo() {return 1;}


int main() {
	int foobar;
	foobar = 12;
	
	barfoo				()	; barfoo		 (	 	 	  	)	 	 ;
	
	barfoo(
	)
	;
	return foo
	(
	0x3 
	& 
			3
)
	+bar	  	 	 	( 	45 + 't'	 	 	 	 	 	 ,
	
	
	foobar
,
			   foobar + 3 & 0xff
			  ) - 1;
}