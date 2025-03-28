#pragma once
#include "headers.h"
using namespace std;

//Utilities necessary for the hooks
BOOL prepare_setup();
string wstring_tostring(wstring ws);
wstring string_towstring(string& str);
map<string, string> process_filename(string original_filename);
string getTime();
int call_childLogger(string log);
void saveBeforeDelete(LPCSTR lpfilename);