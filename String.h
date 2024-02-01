#ifndef STRING_H
#define STRING_H

#include <map>
#include <string>
#include <cstring>

#include "Functions.h"
#include "Variables.h"

void registerMethod_String(map<string, Functions>* functions_available, string name);
void registerAll_String(map<string, Functions>* functions_available);
bool inList_String(string name);

#endif // !STRING_H
