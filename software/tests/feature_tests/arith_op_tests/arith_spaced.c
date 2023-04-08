// Check arithmetic operators allow for odd spacing.
int main() {
	int var;
	
	var = -3;
	
	int add1;
	int add2;
	int sub1;
	int sub2;
	int minus1;
	int minus2;
	
	add1 =					  5+	var;
	add2 =6+7;
	
	sub1=
	var
	-	
	     3;
	sub2 =                        10  	 	 -var;
	
	minus1 = -	 	 		  	 var;
	minus2 = -
	5;
	
	// 2 + 13 + -6 + 13 + 3 + -5 = 20
	return add1 + add2 + sub1 + sub2 + minus1 + minus2 + -19;
}