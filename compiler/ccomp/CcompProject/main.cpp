/*
 * main.cpp: Head of the program/application.
 */

#include <iostream>
#include <cstdint>
#include <string>

using namespace std;

// TODO
#include "Common/Device/Print.h"
#include "Common/Util/Msg.h"
#include "Common/Util/Ptr.h"


//==============================================================================
// Start/main process of the program.
int main(int argc, char* argv[]) {

	/*
	struct foo {
		int f = 3;
		string b = "bar";
		bool o = false;

		~foo(void) {Print_cli("down");}
	};

	Ptr<struct foo> ptr(new struct foo());
	Ptr<struct foo> nwe(nullptr);

	if (ptr.isNull() == false) {Print_cli("FALSE");}
	else                       {Print_cli("TRUE");}

	string str = Msg() + ptr.refCnt() + " and " + ptr->f + " with " + (*ptr).b + " lastly ";
	Print_cli(str);

	//string m = Msg() + (*nwe).b;

	ptr = nwe;

	Print_cli((string)(Msg() + ptr.refCnt()));

	ptr.~Ptr();
	*/
	Ptr<int> ope(nullptr);

	Print_cli("END");
	return 0;
}
