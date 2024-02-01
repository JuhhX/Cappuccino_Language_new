#ifndef GENERAL_H
#define GENERAL_H

#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <map>

#include "Functions.h"
#include "StringUtilities.h"
#include "Variables.h"
#include "NativeFunctions.h"
#include "Classes.h"

using namespace std;

struct Class;

static struct Colors {
	const string BLACK = "\033[1;30m";
	const string RED = "\033[1;31m";
	const string GREEN = "\033[1;32m";
	const string YELLOW = "\033[1;33m";
	const string BLUE = "\033[1;34m";
	const string MAGENTA = "\033[1;35m";
	const string CYAN = "\033[1;36m";
	const string WHITE = "\033[1;37m";
	const string RESET = "\033[1;0m";
} colors;

extern ifstream* file_reference;

enum CommonRequestUpdateVariableMode {
	INCREASE,
	DECREASE
};

struct CommonRequestUpdateVariable {
	string variable_name = "";
	CommonRequestUpdateVariableMode mode;
};

enum StructureType{
	IF,
	ELSE_IF,
	ELSE,
	WHILE,
	FOR,
	LOOP,
	USER_FUNCTION,
	USER_CLASS,
	EXECUTING_FUNCTION,
	VOID_STRUCUTRE
};

enum SatisfiedDecision {
	IF_SATISFIED,
	ELSE_IF_SATISFIED,
	NOT_SATISFIED
};

struct BasicScopesStructures{
	StructureType type = VOID_STRUCUTRE;
	size_t back_in = (size_t)0;
	string controller_variable = "";
	string evaluate_expression = "";
	SatisfiedDecision satisfied = NOT_SATISFIED;
	string creating_function = "";
};

struct VectorReference {
	ReturnCategory type = VOID;
	map<string, int>* integers;
	map<string, double>* doubles;
	map<string, bool>* booleans;
	map<string, char>* chars;
	map<string, string>* strings;
	string name = "";
	int size = 0;
};

extern map<string, int> universal_integer_variables;
extern map<string, double> universal_double_variables;
extern map<string, char> universal_char_variables;
extern map<string, bool> universal_boolean_variables;
extern map<string, string> universal_string_variables;
extern map<string, Class> universal_class_variables;

extern vector<map<string, int>*> references_integer_variables;
extern vector<map<string, double>*> references_double_variables;
extern vector<map<string, char>*> references_char_variables;
extern vector<map<string, bool>*> references_boolean_variables;
extern vector<map<string, string>*> references_string_variables;
extern vector<map<string, Class>*> references_class_variables;

extern vector< map<string, VectorReference>*> references_vector_references_variables;

//extern map<string, vector<int*>> references_integer_vector_variables;

class Scope
{
	public:
		map<string, int> integer_variables;
		map<string, double> double_variables;
		map<string, char> char_variables;
		map<string, bool> boolean_variables;
		map<string, string> string_variables;
		map<string, Class> class_variables;

		map<string, VectorReference> vector_references_variables;

		int curly_braces_count = 0;

		bool ignore_scope = false;
		bool is_reading_function = false;

		//Controle de loops em métodos
		stack<int> stack_method_line;
		int* current_method_line;
		bool block_current_line = false;

		ReturnType current_return;

		BasicScopesStructures basic_structures;
		stack<CommonRequestUpdateVariable> commonRequestUpdateVariable;

		~Scope() {

			if (!this->integer_variables.empty()) {
				this->integer_variables.clear();
			}
			if (!this->double_variables.empty()) {
				this->double_variables.clear();
			}
			if (!this->char_variables.empty()){
				this->char_variables.clear();
			}
			if (!this->boolean_variables.empty()){
				this->boolean_variables.clear();
			}
			if (!this->string_variables.empty()){
				this->string_variables.clear();
			}

			this->current_return.category = VOID;
			this->current_return.returnValue.integer = 0;
			this->current_return.string_ = "";
		}

};

void interpretLine(string line);
void pushVariablesReferences();
void updateVectorsReferencesPointers();
void popVariablesReferences();
extern Scope current_scope;
extern stack<Scope> scopes;

#endif // !GENERAL_H
