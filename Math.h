#ifndef MATH_H
#define MATH_H

#include <map>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include "Functions.h"
#include "Variables.h"

void registerMethod(map<string, Functions>* functions_available, string name);
void registerAll(map<string, Functions>* functions_available);
bool inList(string name);

#endif // !MATH_H
