/*
 * mian.cpp
 *
 *  Created on: Jan 26, 2025
 *      Author: John
 */


#include <iostream>

#include "Foo/foo.h"

using namespace std;

typedef struct {
	int g;
} foo;

int main() {
	foo foo = {
		.g = 3,
	};

	int a = 3;
	foobar(a);
	cout << "ONLINE!" << a << endl << std::flush;
	return 0;
}
