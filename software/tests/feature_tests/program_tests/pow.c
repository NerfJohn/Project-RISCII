// Example program of a basic exponent algorithm.

// Recursive approach to implementing exponentiation.
unsigned int pow(unsigned int x, unsigned int y) {
    // Result to return.
    int result;
    result = 0;
    
    // Determine x^(y-1) first.
    int prev;
    if (y > 2) {prev = pow(x,y-1);}
    if (y == 2) {prev = x;}
    if (y == 1) {return x;}
    if (y == 0) {return 1;}
    
    // Solve for multiplication (ie x^y = x^(y-1) * x).
    int i;
    i = 0;
    while (i < x) {
        result = result + prev;
        i = i + 1;
    }
    
    // Return final result.
    return result;
}

int main() {
    int result;
    
    result = pow(3,4);
    return result == 81;
}