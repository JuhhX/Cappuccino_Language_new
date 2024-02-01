#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <functional>
#include <map>
#include <vector>
#include <stack>

#include "Functions.h"

using namespace std;

enum ReturnCategory {
	INTEGER,
	DOUBLE,
	CHAR,
	BOOLEAN,
	STRING,
	OBJECT,
	INTEGER_VECTOR,
	DOUBLE_VECTOR,
	CHAR_VECTOR,
	BOOLEAN_VECTOR,
	STRING_VECTOR,
	OBJECT_VECTOR,
	VOID
};

union ReturnUnion
{
	int integer;
	double double_;
	char character;
	bool boolean;
};

enum Accessibility {
	PUBLIC,
	PRIVATE,
	PROTECTED
};

struct ReturnType
{
	string string_ = "";
	ReturnCategory category = VOID;
	ReturnUnion returnValue = {0};
};

enum CategoryFunctions {
	NATIVE,
	USER_CREATED,
	USER_CREATED_FUNCTION_CLASS
};

struct Functions
{
	CategoryFunctions category = NATIVE;
	Accessibility accessibility = PUBLIC;
	ReturnCategory returns = VOID;
	function<ReturnType(string)> function;
	string created_params = "";
	string assignature = "";
	string class_owner = "";
	vector<string> created_content;
	bool isClassMethod = false;
};

extern map<string, Functions> functions_available;

ReturnCategory stringToType(string type);
bool isFunction(string params);
bool isFunction(Functions fn, string name);

#endif // ! "FUNCTIONS_H"
