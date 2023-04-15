// Check parentheses can be used to surround expressions.
int main() {
	int lit;
	int unary;
	int expr;
	int var;
	
	lit = (5);
	unary = (!0x0);
	expr = ('\n' + 10);
	var = (expr);
	
	// 5 + 1 + 20 + 20 = 46
	return lit + unary + expr + var + -45;
}