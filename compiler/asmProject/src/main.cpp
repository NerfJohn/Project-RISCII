/*
 * main.cpp
 *
 *  Created on: Jan 1, 2024
 *      Author: John
 */


#include <iostream>

#include "AnalysisData_t.h"
#include "InfoUtils.h"
#include "Parsing/ParseUtils.h"

using namespace std;

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
	// Ensure filenames were provided.
	if (argc < 2) {
		cout << "FATAL ERR: Must provide file(s)" << endl;
		exit(1);
	}

	// Prepare o store program items and info.
	vector<ABuildItem*> prgmItems = {};
	AnalysisData_t prgmData = {
			.m_table = LabelTable(),
			.m_glblLabel = nullptr,
			.m_heapLabel = nullptr,
			.m_textLen = 0,
			.m_dataLen = 0,
			.m_bssLen = 0,

			.m_lastAddrItem = nullptr
	};

	// Process each file (scan/parse/analyze- save to).
	for (int i = 1; i < argc; i++) {
		// Lex file- grouping characters into tokens.
		queue<AsmToken*> tokens = ParseUtils_lexFile(argv[i]);

		// Parse tokens into DS for analysis/translation.
		vector<ABuildItem*> mainDS = ParseUtils_parseTokens(tokens);

		// Analyze the items from the file.
		InfoUtils_analyzeItems(mainDS, &prgmData);

		// File fully checked- push items to overall program storage.
		for (int j = 0; j < (int)(mainDS.size()); j++) {
			prgmItems.push_back(mainDS[j]);
		}
	}

	// Debug info regarding result of pre-translation analysis.
	cout << "INFO: Sizing (";
	cout << "text length = " << prgmData.m_textLen << ", ";
	cout << "data length = " << prgmData.m_dataLen << ", ";
	cout << "bss length = " << prgmData.m_bssLen << ")" << endl;

	// Type check the entire program.
	InfoUtils_typeCheckItems(prgmItems, &prgmData);

	cout << "TODO- Translate" << endl;
}

