/*
 * main.cpp: Head of the program/application.
 */

#include <iostream>
#include <cstdint>
#include <string>

using namespace std;

// TODO
class Msg {
public:
	//Msg(Msg&& s) {m_str = s.m_str; cout << "COPY" << endl;}
	Msg(string foo) {m_str = foo; cout << "STR" << endl;}
	Msg() {cout << "CTOR" << endl;}
	Msg& operator<<(int s) {m_str += to_string(s); return *this;}
	Msg& operator<<(string& s) {m_str += s; return *this;}
	Msg& operator<<(const char* s) {m_str += s; return *this;}
	operator string&() {cout << "TOSTR" << endl; return m_str;}
	//string& toS() {return m_str;}
	~Msg() {m_str = "ball"; cout << "DEL" << endl;}
private:
	string m_str;
};


//==============================================================================
// Start/main process of the program.
int main(int argc, char* argv[]) {

	int value = 42;
	string bar = "yep";
	string foo = Msg("str") << value;

	string qq = Msg() << "my own = '" << 32 << "'";

	cout << foo << endl;
	cout << bar << endl;
	cout << qq << endl;

	return 0;
}
