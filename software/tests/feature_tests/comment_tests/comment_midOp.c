// Check comments can "interrupt" expressions.
int main() {
	int value;
	
	value = 4 	// Attend the tale of Sweeney Todd
	+ 			// His face was pale, his eye was odd
	(			// He shaved the faces of gentlemen
	0x3 		// Who never thereafter were heard from again
	& 			// He trod a path that few have trod
	1			// Did Sweeney Todd
	) 			// The demon barber of
	== 			// Fleet.
	5			// Street.
	;
	
	return value;
}