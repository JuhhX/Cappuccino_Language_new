#ifndef CLASSES_H
#define CLASSES_H

#include <string>
#include <map>

#include "Functions.h"
#include "StringUtilities.h"
#include "General.h"
#include "Variables.h"

using namespace std;

struct Class {
	//ATRIBUTOS PRIVADOS DA CLASSE
	string variable_name = "";

	map<string, int> integer_variables;
	map<string, double> double_variables;
	map<string, char> char_variables;
	map<string, bool> boolean_variables;
	map<string, string> string_variables;

	map<string, int>* pub_integer_variables_references;
	map<string, double>* pub_double_variables_references;
	map<string, char>* pub_char_variables_references;
	map<string, bool>* pub_boolean_variables_references;
	map<string, string>* pub_string_variables_references;

	map<string, Functions> functions_available;
};

struct ClassModel : Class
{
	std::string typename_ = "";
	map<string, int> pub_integer_variables;
	map<string, double> pub_double_variables;
	map<string, char> pub_char_variables;
	map<string, bool> pub_boolean_variables;
	map<string, string> pub_string_variables;
};

class Scope;

extern ClassModel creating_class_model;
extern map<string, ClassModel> classes_models;
extern map<string, Class> classes_variables;

bool isTargetClass(string params);
bool isClassVariable(string line);
void loadClassVariables(string name, Scope* scope);
void unloadClassVariables(string name);

#endif // !CLASSES_H
