#ifndef NATIVEFUNCTIONS_H
#define NATIVEFUNCTIONS_H

#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include "Functions.h"
#include "StringUtilities.h"
#include "Variables.h"
#include "General.h"
#include "Classes.h"

using namespace std;

void loadNativeFunctions();
ReturnType execute_custom(Functions func, string params);
bool isImport(string params);
bool inList(string name);

#endif // !NATIVEFUNCTIONS_H
