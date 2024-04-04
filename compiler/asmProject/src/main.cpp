/*
 * main.cpp
 *
 *  Created on: Jan 1, 2024
 *      Author: John
 */


#include <iostream>

#include "Parsing/ParseUtils.h"

using namespace std;


// TODO- example of scanning file input into scanned/parsed data structure.
void syntaxMain(int argc, char* argv[]) {
	// Ensure filenames were provided.
	if (argc < 2) {
		cout << "FATAL ERR: Must provide file(s)" << endl;
		exit(1);
	}

	// Process each file.
	for (int i = 1; i < argc; i++) {
		// Lex file- grouping characters into tokens.
		queue<AsmToken*> tokens = ParseUtils_lexFile(argv[i]);

		// Parse tokens into DS for analysis/translation.
		vector<ABuildItem*> mainDS = ParseUtils_parseTokens(tokens);
	}
}

/*
 * == Main() Program ==
 * Start of the program. Defers to selected example program.
 *
 * Comment/Uncomment calls to example programs to select which one is desired in
 * the moment.
 *
 * @param argc number of arguments/strings under argv
 * @param argv array of strings/arguments in left-right call order
 * @return general code indicating result, 0 is typically a success
 */
int main(int argc, char* argv[]) {
	// Run example program.
	syntaxMain(argc, argv);

	// Not really intended to hit- may as well return "success".
	return 0;
}

