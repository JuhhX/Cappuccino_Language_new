#ifndef VARIABLES_H
#define VARIABLES_H

#include <string>
#include <vector>
#include <typeinfo>

#include "General.h"
#include "Functions.h"
#include "StringUtilities.h"
#include "Evaluations.h"

using namespace std;

bool isVariableDeclaration(string params);
void declareVariable(string params);

bool isVariableInReferences(string name);
bool isVariable(string name);
bool isVariableUpdate(string name);

string getVariableValue(string params);
ReturnCategory getVariableType(string name);
ReturnCategory getDataType(string s);

//FUNÇÕES GERAIS
template <typename T>
bool isDefaultVectorDeclare(string variable_name, map<string, T>* variables_vector, ReturnCategory category);
bool isDefaultVectorStringDeclare(string variable_name, map<string, string>* variables_vector);
void localInterpreter(string line);

template <typename T>
void updateVariable(string name, T value, ReturnCategory type);
void updateVariable(string name, string value, ReturnCategory type);

void resolveVector(string* name);

bool isVector(string name);
#endif // !VARIABLES_H
