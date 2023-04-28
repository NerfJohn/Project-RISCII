// Check return statements work for unconventional spacing.
int bipity() {return


			1	  		;}

int bopity() {			 	 	
															 return						1;}
															 
int boop() 
{return 1;}

int main() {
	return bipity() + bopity() + boop() - 2;