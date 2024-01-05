/*
 * main.cpp
 *
 *  Created on: Jan 1, 2024
 *      Author: John
 */


#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
	// Print out passed arguments.
	cout << "Arguments:" << endl;
	for (int i = 1; i < argc; i++) {
		cout << argv[i] << endl;
	}
}

